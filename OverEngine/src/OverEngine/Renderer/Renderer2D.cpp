#include "pcheader.h"
#include "Renderer2D.h"

#include "Texture.h"

namespace OverEngine
{
	Renderer2D::Statistics Renderer2D::s_Statistics;

	struct Vertex
	{
		Vector3 a_Position0   = Vector3(0.0f);
		Vector3 a_Position1   = Vector3(0.0f);
		Vector3 a_Position2   = Vector3(0.0f);
		Vector3 a_Position3   = Vector3(0.0f);

		Color   a_Color       = Color(1.0f);

		Vector2    a_TexTiling   = Vector2(1.0f);
		Vector2    a_TexOffset   = Vector2(0.0f);
		int        a_TexFlip     = 0;
		int        a_TexSlot     = -1;
		int        a_TexFilter   = 0;
		glm::ivec2 a_TexWrapping = glm::ivec2(0);
		Vector2    a_TexSize     = Vector2(0.0f);
		Rect       a_TexRect     = Rect(0.0f);
	};

	struct Renderer2DData
	{
		Ref<VertexArray> vertexArray = nullptr;
		Ref<VertexBuffer> vertexBuffer = nullptr;
		Ref<IndexBuffer> indexBuffer = nullptr;

		Vector<Vertex> Vertices;
		Vector<uint32_t> Indices;
		Vector<float> QuadsZIndices;

		uint32_t QuadCapacity;
		uint32_t FlushingQuadCount;

		uint32_t OpaqueInsertIndex = 0;

		Ref<Shader> BatchRenderer2DShader = nullptr;

		UnorderedMap<uint32_t, Ref<GAPI::Texture2D>> TextureBindList;

		Mat4x4 ViewProjectionMatrix;

		static constexpr int ShaderSampler2Ds[32] = {
			0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 ,
			8 , 9 , 10, 11, 12, 13, 14, 15,
			16, 17, 18, 19, 20, 21, 22, 23,
			24, 25, 26, 27, 28, 29, 30, 31
		};
	};

	static Renderer2DData* s_Data;

	static void InsertVertex(bool transparent, const float& z, const Vertex& vertex)
	{
		if (transparent)
		{
			for (auto idx = s_Data->Vertices.begin() + s_Data->OpaqueInsertIndex; idx < s_Data->Vertices.end(); idx++)
			{
				auto i = idx - s_Data->Vertices.begin();
				if (z < s_Data->QuadsZIndices[i])
				{
					s_Data->Vertices.emplace(idx, vertex);
					s_Data->QuadsZIndices.emplace(s_Data->QuadsZIndices.begin() + i, z);
					return;
				}
			}

			s_Data->Vertices.emplace_back(vertex);
			s_Data->QuadsZIndices.emplace_back(z);
			return;
		}

		// Opaque
		s_Data->Vertices.emplace(s_Data->Vertices.begin(), vertex);
		s_Data->QuadsZIndices.emplace(s_Data->QuadsZIndices.begin(), z);
		s_Data->OpaqueInsertIndex++;
	}

	static void GenIndices(uint32_t quadCount, uint32_t indexCount)
	{
		// Allocate storage
		s_Data->indexBuffer->AllocateStorage(indexCount);
		s_Data->Indices.reserve(quadCount);

		for (uint32_t i = (uint32_t)s_Data->Indices.size(); i < quadCount; i++)
		{
			// Generate indices
			s_Data->Indices.push_back(i);
		}

		// Upload data to GPU
		s_Data->indexBuffer->BufferSubData((uint32_t*)s_Data->Indices.data(), indexCount);
	}

	void Renderer2D::Init(uint32_t initQuadCapacity)
	{
		s_Data = new Renderer2DData();

		s_Data->vertexArray = VertexArray::Create();
		s_Data->QuadCapacity = initQuadCapacity;

		s_Data->vertexBuffer = VertexBuffer::Create();
		s_Data->vertexBuffer->AllocateStorage(initQuadCapacity * sizeof(Vertex));
		s_Data->vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position0" },
			{ ShaderDataType::Float3, "a_Position1" },
			{ ShaderDataType::Float3, "a_Position2" },
			{ ShaderDataType::Float3, "a_Position3" },

			{ ShaderDataType::Float4, "a_Color" },

			{ ShaderDataType::Float2, "a_TexTiling" },
			{ ShaderDataType::Float2, "a_TexOffset" },
			{ ShaderDataType::Int, "a_TexFlip" },
			{ ShaderDataType::Int, "a_TexSlot" },
			{ ShaderDataType::Int, "a_TexFilter" },
			{ ShaderDataType::Int2, "a_TexWrapping" },
			{ ShaderDataType::Float2, "a_TexSize" },
			{ ShaderDataType::Float4, "a_TexRect" },
		});
		s_Data->vertexArray->AddVertexBuffer(s_Data->vertexBuffer);

		s_Data->indexBuffer = IndexBuffer::Create();
		s_Data->indexBuffer->AllocateStorage(initQuadCapacity);
		s_Data->vertexArray->SetIndexBuffer(s_Data->indexBuffer);

		s_Data->Vertices.reserve(initQuadCapacity);
		GenIndices(initQuadCapacity, initQuadCapacity);
		s_Data->QuadsZIndices.reserve(initQuadCapacity);

		s_Data->BatchRenderer2DShader = Shader::Create("assets/shaders/BatchRenderer2D.glsl");
		s_Data->BatchRenderer2DShader->Bind();
		s_Data->BatchRenderer2DShader->UploadUniformIntArray("u_Slots", Renderer2DData::ShaderSampler2Ds, 32);

		s_Statistics.Reset();
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	Ref<Shader>& Renderer2D::GetShader()
	{
		return s_Data->BatchRenderer2DShader;
	}

	void Renderer2D::Reset()
	{
		s_Data->Vertices.clear();
		s_Data->QuadsZIndices.clear();
		s_Data->OpaqueInsertIndex = 0;
		s_Data->FlushingQuadCount = 0;
		s_Data->TextureBindList.clear();

		s_Statistics.Reset();
	}

	void Renderer2D::BeginScene(const Mat4x4& viewProjectionMatrix)
	{
		Reset();
		s_Data->ViewProjectionMatrix = viewProjectionMatrix;
	}

	void Renderer2D::BeginScene(const Mat4x4& viewMatrix, const Camera& camera)
	{
		Reset();
		s_Data->ViewProjectionMatrix = camera.GetProjection() * viewMatrix;
	}

	void Renderer2D::BeginScene(const Mat4x4& viewMatrix, const Mat4x4& projectionMatrix)
	{
		Reset();
		s_Data->ViewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::Flush()
	{
		if (s_Data->FlushingQuadCount == 0) // Nothing to draw
			return;

		uint32_t indexCount = s_Statistics.GetIndexCount();

		// Grow GPU Buffers
		if (s_Data->QuadCapacity < s_Data->FlushingQuadCount)
		{
			s_Data->QuadCapacity = s_Data->FlushingQuadCount;
			s_Data->vertexBuffer->AllocateStorage(s_Data->QuadCapacity * sizeof(Vertex));
			GenIndices(s_Data->QuadCapacity, indexCount);
		}

		// Upload Data
		s_Data->vertexBuffer->BufferSubData((void*)s_Data->Vertices.data(), s_Statistics.GetVertexCount() * sizeof(Vertex));

		// Bind Textures
		for (auto& t : s_Data->TextureBindList)
			t.second->Bind(t.first);

		// Bind VertexArray
		s_Data->vertexArray->Bind();

		// Bind Shader
		s_Data->BatchRenderer2DShader->Bind();

		// DrawCall
		RenderCommand::DrawIndexed(s_Data->vertexArray, indexCount, DrawType::Points);
		s_Statistics.DrawCalls++;
	}

	void Renderer2D::FlushAndReset()
	{
		Flush();

		s_Data->Vertices.clear();
		s_Data->QuadsZIndices.clear();
		s_Data->OpaqueInsertIndex = 0;
		s_Data->FlushingQuadCount = 0;
		s_Data->TextureBindList.clear();
	}

	/////////////////////////////////////////////////////////
	// FlatColor Quad ///////////////////////////////////////
	/////////////////////////////////////////////////////////

	void Renderer2D::DrawQuad(const Vector2& position, float rotation, const Vector2& size, const Color& color)
	{
		DrawQuad(Vector3(position, 0.0f), rotation, size, color);
	}

	void Renderer2D::DrawQuad(const Vector3& position, float rotation, const Vector2& size, const Color& color)
	{
		Mat4x4 transform =
			glm::translate(Mat4x4(1.0f), position) *
			glm::rotate(Mat4x4(1.0f), rotation, Vector3(0, 0, 1)) *
			glm::scale(Mat4x4(1.0f), Vector3(size, 1.0f));

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const Mat4x4& transform, const Color& color)
	{
		if (color.a  == 0)
			return;

		bool transparent = color.a < 1.0f;

		Vertex vertex;

		auto mat = s_Data->ViewProjectionMatrix * transform;

		vertex.a_Position0 = Vector3(mat * Vector4(-0.5, -0.5, 0.0, 1.0));
		vertex.a_Position1 = Vector3(mat * Vector4( 0.5, -0.5, 0.0, 1.0));
		vertex.a_Position2 = Vector3(mat * Vector4(-0.5,  0.5, 0.0, 1.0));
		vertex.a_Position3 = Vector3(mat * Vector4( 0.5,  0.5, 0.0, 1.0));

		vertex.a_Color = color;

		InsertVertex(transparent, mat[3].z, vertex);

		s_Statistics.QuadCount++;
		s_Data->FlushingQuadCount++;
	}

	/////////////////////////////////////////////////////////
	// Textured Quad ////////////////////////////////////////
	/////////////////////////////////////////////////////////

	void Renderer2D::DrawQuad(const Vector2& position, float rotation, const Vector2& size, Ref<Texture2D> texture, const TexturedQuadExtraData& extraData)
	{
		DrawQuad(Vector3(position, 0.0f), rotation, size, texture, extraData);
	}

	void Renderer2D::DrawQuad(const Vector3& position, float rotation, const Vector2& size, Ref<Texture2D> texture, const TexturedQuadExtraData& extraData)
	{
		Mat4x4 transform =
			glm::translate(Mat4x4(1.0f), position) *
			glm::rotate(Mat4x4(1.0f), rotation, Vector3(0, 0, 1)) *
			glm::scale(Mat4x4(1.0f), Vector3(size, 1.0f));

		DrawQuad(transform, texture, extraData);
	}

	void Renderer2D::DrawQuad(const Mat4x4& transform, Ref<Texture2D> texture, const TexturedQuadExtraData& extraData)
	{
		if (!texture || texture->GetType() == TextureType::Placeholder)
			return;

		if (extraData.Tint.a == 0)
			return;

		auto textureToBind = texture->GetGPUTexture();

		int8_t textureSlot = -1;
		for (const auto& t : s_Data->TextureBindList)
		{
			if (*t.second == *textureToBind)
			{
				textureSlot = t.first;
				break;
			}
		}

		if (textureSlot == -1)
		{
			uint32_t slot = (uint32_t)s_Data->TextureBindList.size();
			if (slot + 1 > RenderCommand::GetMaxTextureSlotCount())
			{
				Flush();
				Reset();
				slot = 0;
			}
			s_Data->TextureBindList[slot] = textureToBind;
			textureSlot = slot;
		}

		bool transparent = extraData.Tint.a < 1.0f || texture->GetFormat() == TextureFormat::RGBA;

		Vertex vertex;

		auto mat = s_Data->ViewProjectionMatrix * transform;

		vertex.a_Position0 = Vector3(mat * Vector4(-0.5, -0.5, 0.0, 1.0));
		vertex.a_Position1 = Vector3(mat * Vector4( 0.5, -0.5, 0.0, 1.0));
		vertex.a_Position2 = Vector3(mat * Vector4(-0.5,  0.5, 0.0, 1.0));
		vertex.a_Position3 = Vector3(mat * Vector4( 0.5,  0.5, 0.0, 1.0));

		vertex.a_Color = extraData.Tint;

		vertex.a_TexTiling = extraData.Tiling;
		vertex.a_TexOffset = extraData.Offset;
		vertex.a_TexFlip = extraData.Flip;
		vertex.a_TexSlot = textureSlot;
		
		// a_TexFilter
		if (extraData.Filtering != TextureFiltering::None)
			vertex.a_TexFilter = (int)extraData.Filtering;
		else
			vertex.a_TexFilter = (int)texture->GetFiltering();

		// a_TexSWrapping & a_TexTWrapping
		if (extraData.Wrapping.x != TextureWrapping::None)
			vertex.a_TexWrapping.x = (int)extraData.Wrapping.x;
		else
			vertex.a_TexWrapping.x = (int)texture->GetXWrapping();

		if (extraData.Wrapping.y != TextureWrapping::None)
			vertex.a_TexWrapping.y = (int)extraData.Wrapping.y;
		else
			vertex.a_TexWrapping.y = (int)texture->GetYWrapping();

		if (vertex.a_TexWrapping.x == (int)TextureWrapping::ClampToBorder)
			vertex.a_TexWrapping.x = (int)TextureWrapping::Repeat;

		if (vertex.a_TexWrapping.y == (int)TextureWrapping::ClampToBorder)
			vertex.a_TexWrapping.y = (int)TextureWrapping::Repeat;

		vertex.a_TexSize = { texture->GetWidth() ,texture->GetHeight() };
		vertex.a_TexRect = texture->GetRect();

		InsertVertex(transparent, mat[3].z, vertex);

		s_Statistics.QuadCount++;
		s_Data->FlushingQuadCount++;
	}
}
