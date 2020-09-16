#include "pcheader.h"
#include "Renderer2D.h"

#include "RendererAPi.h"

#include "Texture.h"

namespace OverEngine
{
	Renderer2D::Statistics Renderer2D::s_Statistics;

	struct Vertex
	{
		Vector4 a_Position = Vector4(0.0f);
		Color a_Color = Color(0.0f);

		float a_TextureSlot                   = -1.0f;
		float a_TextureFilter                 = 0.0f;
		float a_TextureAlphaClippingThreshold = 0.0f;
		Vector2 a_TextureWrapping             = Vector2(0.0f);
		Color a_TextureBorderColor            = Color(0.0f);
		Rect a_TextureRect                    = Rect(0.0f);
		Vector2 a_TextureSize                 = Vector2(0.0f);
		Vector2 a_TextureCoord                = Vector2(0.0f);
	};

	using DrawQuadVertices = std::array<Vertex, 4>;
	using DrawQuadIndices = std::array<uint32_t, 6>;

	struct Renderer2DData
	{
		Ref<VertexArray> vertexArray = nullptr;
		Ref<VertexBuffer> vertexBuffer = nullptr;
		Ref<IndexBuffer> indexBuffer = nullptr;

		Vector<DrawQuadVertices> Vertices;
		Vector<DrawQuadIndices> Indices;
		Vector<float> IndicesZ;
		uint32_t QuadCapacity;

		uint32_t OpaqueInsertIndex = 0;

		Ref<Shader> BatchRenderer2DShader = nullptr;

		UnorderedMap<uint32_t, Ref<GAPI::Texture2D>> TextureBindList;

		Mat4x4 ViewProjectionMatrix;

		static constexpr float QuadVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		static constexpr uint32_t QuadIndices[6] = { 0, 1, 2, 2, 3, 0 };

		static constexpr int ShaderSampler2Ds[32] = {
			0, 1, 2, 3, 4, 5, 6, 7,
			8, 9, 10, 11, 12, 13, 14, 15,
			16, 17, 18, 19, 20, 21, 22, 23,
			24, 25, 26, 27, 28, 29, 30, 31
		};
	};

	static Renderer2DData* s_Data;

	static void InsertIndices(bool transparent, const float& z, const DrawQuadIndices& indices)
	{
		if (transparent)
		{
			for (auto idx = s_Data->Indices.begin() + s_Data->OpaqueInsertIndex; idx < s_Data->Indices.end(); idx++)
			{
				auto i = idx - s_Data->Indices.begin();
				if (z <= s_Data->IndicesZ[i])
				{
					s_Data->Indices.emplace(idx, indices);
					s_Data->IndicesZ.emplace(s_Data->IndicesZ.begin() + i, z);
					return;
				}
			}

			s_Data->Indices.emplace_back(indices);
			s_Data->IndicesZ.emplace_back(z);
			return;
		}

		// Opaque
		s_Data->Indices.emplace(s_Data->Indices.begin() + s_Data->OpaqueInsertIndex, indices);
		s_Data->IndicesZ.emplace(s_Data->IndicesZ.begin() + s_Data->OpaqueInsertIndex, z);
		s_Data->OpaqueInsertIndex++;
	}

	void Renderer2D::Init(uint32_t initQuadCapacity)
	{
		s_Data = new Renderer2DData();

		s_Data->vertexArray = VertexArray::Create();
		s_Data->QuadCapacity = initQuadCapacity;

		s_Data->vertexBuffer = VertexBuffer::Create();
		s_Data->vertexBuffer->AllocateStorage(initQuadCapacity * 4 * sizeof(Vertex));
		s_Data->vertexBuffer->SetLayout({
			{ ShaderDataType::Float4, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },

			{ ShaderDataType::Float, "a_TextureSlot" },
			{ ShaderDataType::Float, "a_TextureFilter" },
			{ ShaderDataType::Float, "a_TextureAlphaClippingThreshold" },
			{ ShaderDataType::Float2, "a_TextureWrapping" },
			{ ShaderDataType::Float4, "a_TextureBorderColor" },
			{ ShaderDataType::Float4, "a_TextureRect" },
			{ ShaderDataType::Float2, "a_TextureSize" },
			{ ShaderDataType::Float2, "a_TextureCoord" },
		});
		s_Data->vertexArray->AddVertexBuffer(s_Data->vertexBuffer);

		s_Data->indexBuffer = IndexBuffer::Create();
		s_Data->indexBuffer->AllocateStorage(initQuadCapacity * 6);
		s_Data->vertexArray->SetIndexBuffer(s_Data->indexBuffer);

		s_Data->Vertices.reserve(initQuadCapacity);
		s_Data->Indices.reserve(initQuadCapacity);
		s_Data->IndicesZ.reserve(initQuadCapacity);

		s_Data->BatchRenderer2DShader = Shader::Create("assets/shaders/BatchRenderer2D.glsl");
		//s_Data->BatchRenderer2DShader = Shader::Create("BatchRenderer2D", BatchRenderer2DVertexShaderSrc, BatchRenderer2DFragmentShaderSrc);
		s_Data->BatchRenderer2DShader->Bind();
		s_Data->BatchRenderer2DShader->UploadUniformIntArray("u_Slots", Renderer2DData::ShaderSampler2Ds, 32);

		s_Statistics.Reset();
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::Reset()
	{
		s_Data->Vertices.clear();
		s_Data->Indices.clear();
		s_Data->IndicesZ.clear();
		s_Data->OpaqueInsertIndex = 0;

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
		s_Data->ViewProjectionMatrix = camera.GetProjectionMatrix() * viewMatrix;
	}

	void Renderer2D::BeginScene(const Mat4x4& viewMatrix, const Mat4x4& projectionMatrix)
	{
		Reset();
		s_Data->ViewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	void Renderer2D::EndScene()
	{
		if (!s_Statistics.QuadCount) // Nothing to draw
			return;

		// Grow GPU Buffers
		if (s_Data->QuadCapacity < s_Statistics.QuadCount)
		{
			s_Data->QuadCapacity = s_Statistics.QuadCount;
			s_Data->vertexBuffer->AllocateStorage(s_Data->QuadCapacity * 4 * sizeof(Vertex));
			s_Data->indexBuffer->AllocateStorage(s_Data->QuadCapacity * 6);
		}

		uint32_t indexCount = s_Statistics.GetIndexCount();

		// Upload Data
		s_Data->vertexBuffer->BufferSubData((float*)s_Data->Vertices.data(), s_Statistics.GetVertexCount() * sizeof(Vertex));
		s_Data->indexBuffer->BufferSubData((uint32_t*)s_Data->Indices.data(), indexCount);

		// Bind Textures
		for (auto& t : s_Data->TextureBindList)
			t.second->Bind(t.first);

		// Bind VertexArray
		s_Data->vertexArray->Bind();

		// Bind Shader
		s_Data->BatchRenderer2DShader->Bind();

		// DrawCall
		RenderCommand::DrawIndexed(s_Data->vertexArray, indexCount);
		s_Statistics.DrawCalls++;
	}

	/////////////////////////////////////////////////////////
	// FlatColor Quad ///////////////////////////////////////
	/////////////////////////////////////////////////////////

	void Renderer2D::DrawQuad(const Vector2& position, float rotation, const Vector2& size, const Color& color, float alphaClippingThreshold)
	{
		DrawQuad(Vector3(position, 0.0f), rotation, size, color, alphaClippingThreshold);
	}

	void Renderer2D::DrawQuad(const Vector3& position, float rotation, const Vector2& size, const Color& color, float alphaClippingThreshold)
	{
		Mat4x4 transform =
			glm::translate(Mat4x4(1.0f), position) *
			glm::rotate(Mat4x4(1.0f), rotation, Vector3(0, 0, 1)) *
			glm::scale(Mat4x4(1.0f), Vector3(size, 1.0f));

		DrawQuad(transform, color, alphaClippingThreshold);
	}

	void Renderer2D::DrawQuad(const Mat4x4& transform, const Color& color, float alphaClippingThreshold)
	{
		if (color.a <= alphaClippingThreshold)
			return;

		bool transparent = color.a < 1.0f;

		std::array<Vertex, 4> vertices;

		for (int i = 0; i < 4; i++)
		{
			Vertex& vertex = vertices[i];

			vertex.a_Position.x = Renderer2DData::QuadVertices[0 + 3 * i];
			vertex.a_Position.y = Renderer2DData::QuadVertices[1 + 3 * i];
			vertex.a_Position.z = Renderer2DData::QuadVertices[2 + 3 * i];
			vertex.a_Position.w = 1.0f;
			vertex.a_Position = s_Data->ViewProjectionMatrix * transform * vertex.a_Position;

			vertex.a_Color = color;
		}

		s_Data->Vertices.push_back(vertices);

		DrawQuadIndices indices;
		for (uint32_t i = 0; i < 6; i++)
			indices[i] = Renderer2DData::QuadIndices[i] + 4 * s_Statistics.QuadCount;

		InsertIndices(transparent, 1 - vertices[0].a_Position.z, indices);

		s_Statistics.QuadCount++;
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
		if (extraData.alphaClippingThreshold >= 1.0f || extraData.tint.a <= extraData.alphaClippingThreshold)
			return;

		bool transparent = extraData.tint.a < 1.0f || texture->GetFormat() == TextureFormat::RGBA;

		std::array<Vertex, 4> vertices;

		for (int i = 0; i < 4; i++)
		{
			Vertex& vertex = vertices[i];

			vertex.a_Position.x = Renderer2DData::QuadVertices[0 + 3 * i];
			vertex.a_Position.y = Renderer2DData::QuadVertices[1 + 3 * i];
			vertex.a_Position.z = Renderer2DData::QuadVertices[2 + 3 * i];
			vertex.a_Position.w = 1.0f;
			vertex.a_Position = s_Data->ViewProjectionMatrix * transform * vertex.a_Position;

			vertex.a_Color = extraData.tint;

			// a_TextureSlot
			{
				Ref<GAPI::Texture2D> textureToBind;

				if (texture->GetType() == TextureType::Master)
					textureToBind = texture->m_MasterTextureData.m_MappedTexture;
				else
					textureToBind = texture->m_SubTextureData.m_Parent->m_MasterTextureData.m_MappedTexture;

				bool textureFound = false;
				uint32_t textureSlot = 0;
				for (const auto& t : s_Data->TextureBindList)
				{
					if (*t.second == *textureToBind)
					{
						textureFound = true;
						textureSlot = t.first;
						break;
					}
				}

				if (!textureFound)
				{
					uint32_t slot = (uint32_t)s_Data->TextureBindList.size();
					s_Data->TextureBindList[slot] = textureToBind;
					textureSlot = slot;
				}

				vertex.a_TextureSlot = (float)textureSlot;
			}

			// a_TextureFilter
			if (extraData.overrideTextureFiltering != TextureFiltering::None)
				vertex.a_TextureFilter = (float)extraData.overrideTextureFiltering;
			else
				vertex.a_TextureFilter = (float)texture->GetFilter();

			vertex.a_TextureAlphaClippingThreshold = extraData.alphaClippingThreshold;

			// a_TextureSWrapping & a_TextureTWrapping
			if (extraData.overrideSTextureWrapping != TextureWrapping::None)
				vertex.a_TextureWrapping.x = (float)extraData.overrideSTextureWrapping;
			else
				vertex.a_TextureWrapping.x = (float)texture->GetSWrapping();

			if (extraData.overrideTTextureWrapping != TextureWrapping::None)
				vertex.a_TextureWrapping.y = (float)extraData.overrideTTextureWrapping;
			else
				vertex.a_TextureWrapping.y = (float)texture->GetTWrapping();

			// a_TextureBorderColor
			if (   extraData.overrideTextureBorderColor.r >= 0.0f
				&& extraData.overrideTextureBorderColor.g >= 0.0f
				&& extraData.overrideTextureBorderColor.b >= 0.0f
				&& extraData.overrideTextureBorderColor.a >= 0.0f)
				vertex.a_TextureBorderColor = extraData.overrideTextureBorderColor;
			else
				vertex.a_TextureBorderColor = texture->GetBorderColor();

			// a_TextureRect
			vertex.a_TextureRect = texture->GetRect();

			// a_TextureSize
			vertex.a_TextureSize = { texture->GetWidth() ,texture->GetHeight() };

			// a_TextureCoord
			{
				float xTexCoord = Renderer2DData::QuadVertices[0 + 3 * i] > 0.0f ? extraData.tilingFactorX : 0.0f;
				float yTexCoord = Renderer2DData::QuadVertices[1 + 3 * i] > 0.0f ? extraData.tilingFactorY : 0.0f;
				vertex.a_TextureCoord.x = extraData.flipX ? extraData.tilingFactorX - xTexCoord : xTexCoord;
				vertex.a_TextureCoord.y = extraData.flipY ? extraData.tilingFactorY - yTexCoord : yTexCoord;
			}
		}

		s_Data->Vertices.push_back(vertices);

		DrawQuadIndices indices;
		for (uint32_t i = 0; i < 6; i++)
			indices[i] = Renderer2DData::QuadIndices[i] + 4 * s_Statistics.QuadCount;

		InsertIndices(transparent, 1 - vertices[0].a_Position.z, indices);

		s_Statistics.QuadCount++;
	}
}
