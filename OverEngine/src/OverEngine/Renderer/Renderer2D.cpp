#include "pcheader.h"
#include "Renderer2D.h"

#include "Texture.h"

namespace OverEngine
{
	Renderer2D::Statistics Renderer2D::s_Statistics;

	struct Vertex
	{
		Vector3 a_Position0 = Vector3(0.0f);
		Vector3 a_Position1 = Vector3(0.0f);
		Vector3 a_Position2 = Vector3(0.0f);
		Vector3 a_Position3 = Vector3(0.0f);

		Color   a_Color     = Color(1.0f);
		int     a_TexSlot   = -1;
		Vector4 a_TexCoord  = Vector4(0.0f);
		Vector4 a_TexRegion = Vector4(0.0f);
		int     a_TexRepeat = 0;
	};

	// Hard-coded Limits
	static constexpr uint32_t MaxTextureCount = 32;
	static constexpr uint32_t MaxQuadCount = 1000000;
	
	struct Renderer2DData
	{
		Ref<VertexArray>  QuadVA = nullptr;
		Ref<VertexBuffer> QuadVB = nullptr;

		Vertex* QuadBufferBasePtr = nullptr;
		Vertex* QuadBufferPtr = nullptr;

		uint32_t OpaqueInsertIndex = 0;

		uint32_t QuadCount = 0;
		uint8_t TextureCount = 0;

		Ref<OverEngine::Shader> Shader = nullptr;

		std::array<Ref<Texture2D>, MaxTextureCount> TextureBindList;

		Mat4x4 ViewProjectionMatrix;
	};

	static Renderer2DData* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DData();

		s_Data->QuadVA = VertexArray::Create();

		s_Data->QuadVB = VertexBuffer::Create();
		s_Data->QuadVB->AllocateStorage(MaxQuadCount * sizeof(Vertex));
		s_Data->QuadVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position0" },
			{ ShaderDataType::Float3, "a_Position1" },
			{ ShaderDataType::Float3, "a_Position2" },
			{ ShaderDataType::Float3, "a_Position3" },

			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Int, "a_TexSlot" },
			{ ShaderDataType::Float4, "a_TexCoord" },
			{ ShaderDataType::Float4, "a_TexRegion" },
			{ ShaderDataType::Int, "a_TexRepeat" }
		});
		s_Data->QuadVA->AddVertexBuffer(s_Data->QuadVB);

		{
			auto quadIB = IndexBuffer::Create();
			uint32_t* indices = new uint32_t[MaxQuadCount];

			for (uint32_t i = 0; i < MaxQuadCount; i++)
				indices[i] = i;

			quadIB->BufferData(indices, MaxQuadCount);
			delete[] indices;

			s_Data->QuadVA->SetIndexBuffer(quadIB);
		}

		s_Data->QuadBufferBasePtr = new Vertex[MaxQuadCount];
		s_Data->QuadBufferPtr = s_Data->QuadBufferBasePtr;

		s_Data->Shader = Shader::Create("assets/shaders/BatchRenderer2D.glsl");
		{
			int textureIDs[MaxTextureCount];

			for (int i = 0; i < (int)MaxTextureCount; i++)
				textureIDs[i] = i;

			s_Data->Shader->Bind();
			s_Data->Shader->UploadUniformIntArray("u_Slots", textureIDs, MaxTextureCount);
		}

		s_Statistics.Reset();
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_Data->QuadBufferBasePtr;
		delete s_Data;
	}

	Ref<Shader>& Renderer2D::GetShader()
	{
		return s_Data->Shader;
	}

	void Renderer2D::Reset()
	{
		s_Data->QuadBufferPtr = s_Data->QuadBufferBasePtr;
		s_Data->OpaqueInsertIndex = 0;
		s_Data->QuadCount = 0;
		s_Data->TextureCount = 0;

		s_Statistics.Reset();
	}

	void Renderer2D::BeginScene(const Mat4x4& viewMatrix, const Camera& camera)
	{
		s_Data->ViewProjectionMatrix = camera.GetProjection() * viewMatrix;
		Reset();
		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_Data->QuadBufferPtr = s_Data->QuadBufferBasePtr;
		s_Data->OpaqueInsertIndex = 0;
		s_Data->QuadCount = 0;
		s_Data->TextureCount = 0;
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::Flush()
	{
		// Nothing to draw
		if (s_Data->QuadCount == 0)
			return;

		std::sort(s_Data->QuadBufferBasePtr, s_Data->QuadBufferBasePtr + s_Data->QuadCount + 1, [](const Vertex& a, const Vertex& b)
		{
			return a.a_Position0.z > b.a_Position0.z;
		});

		// Upload Data
		s_Data->QuadVB->BufferSubData((void*)s_Data->QuadBufferBasePtr, s_Data->QuadCount * sizeof(Vertex));

		// Bind Textures
		for (uint8_t i = 0; i < s_Data->TextureCount; i++)
			s_Data->TextureBindList[i]->Bind(i);

		// Bind VertexArray & Shader
		s_Data->QuadVA->Bind();
		s_Data->Shader->Bind();

		// DrawCall
		RenderCommand::DrawIndexed(s_Data->QuadVA, s_Data->QuadCount, DrawType::Points);
		s_Statistics.DrawCalls++;
	}

	////////////////////////////////////////////////////////
	/// FlatColor Quad /////////////////////////////////////
	////////////////////////////////////////////////////////

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
		if (color.a == 0)
			return;

		if (s_Data->QuadCount + 1 >= MaxQuadCount)
			NextBatch();

		auto mat = s_Data->ViewProjectionMatrix * transform;

		s_Data->QuadBufferPtr->a_Position0 = Vector3(mat * Vector4(-0.5, -0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr->a_Position1 = Vector3(mat * Vector4( 0.5, -0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr->a_Position2 = Vector3(mat * Vector4(-0.5,  0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr->a_Position3 = Vector3(mat * Vector4( 0.5,  0.5, 0.0, 1.0));

		s_Data->QuadBufferPtr->a_Color = color;
		s_Data->QuadBufferPtr->a_TexSlot = -1;

		s_Data->QuadBufferPtr++;
		s_Data->QuadCount++;
		s_Statistics.QuadCount++;
	}

	////////////////////////////////////////////////////////
	/// Textured Quad //////////////////////////////////////
	////////////////////////////////////////////////////////

	void Renderer2D::DrawQuad(const Vector2& position, float rotation, const Vector2& size, const TexturedQuadProps& props)
	{
		DrawQuad(Vector3(position, 0.0f), rotation, size, props);
	}

	void Renderer2D::DrawQuad(const Vector3& position, float rotation, const Vector2& size, const TexturedQuadProps& props)
	{
		Mat4x4 transform =
			glm::translate(Mat4x4(1.0f), position) *
			glm::rotate(Mat4x4(1.0f), rotation, Vector3(0, 0, 1)) *
			glm::scale(Mat4x4(1.0f), Vector3(size, 1.0f));

		DrawQuad(transform, props);
	}

	void Renderer2D::DrawQuad(const Mat4x4& transform, const TexturedQuadProps& props)
	{
		if (!props.Sprite)
			return;

		if (props.Tint.a == 0)
			return;

		if (s_Data->QuadCount + 1 >= MaxQuadCount)
			NextBatch();

		{
			Ref<Texture2D> gpuTex = (props.Sprite->GetType() == TextureType::SubTexture) ? std::dynamic_pointer_cast<SubTexture2D>(props.Sprite)->GetMasterTexture() : props.Sprite;

			auto end = s_Data->TextureBindList.begin() + s_Data->TextureCount;
			auto it = std::find(s_Data->TextureBindList.begin(), end, gpuTex);
			if (it == end)
			{
				uint8_t slot = s_Data->TextureCount;
				if (slot + 1u > RenderCommand::GetMaxTextureSlotCount())
				{
					NextBatch();
					slot = 0;
				}
				s_Data->TextureBindList[slot] = gpuTex;
				s_Data->TextureCount++;
				s_Data->QuadBufferPtr->a_TexSlot = slot;
			}
			else
			{
				s_Data->QuadBufferPtr->a_TexSlot = static_cast<int>(it - s_Data->TextureBindList.begin());
			}
		}

		auto mat = s_Data->ViewProjectionMatrix * transform;

		s_Data->QuadBufferPtr->a_Position0 = Vector3(mat * Vector4(-0.5, -0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr->a_Position1 = Vector3(mat * Vector4( 0.5, -0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr->a_Position2 = Vector3(mat * Vector4(-0.5,  0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr->a_Position3 = Vector3(mat * Vector4( 0.5,  0.5, 0.0, 1.0));

		s_Data->QuadBufferPtr->a_Color = props.Tint;
		s_Data->QuadBufferPtr->a_TexCoord = (props.Sprite->GetType() == TextureType::Master) ? Vector4(0, 0, 1, 1) : std::dynamic_pointer_cast<SubTexture2D>(props.Sprite)->GetRect();
		s_Data->QuadBufferPtr->a_TexRepeat = props.ForceTile;

		if (props.ForceTile)
		{
			s_Data->QuadBufferPtr->a_TexRegion = s_Data->QuadBufferPtr->a_TexCoord;

			s_Data->QuadBufferPtr->a_TexRegion.x += (props.Flip & TextureFlip_X) * s_Data->QuadBufferPtr->a_TexRegion.z;
			s_Data->QuadBufferPtr->a_TexRegion.z *= -1 + (int)(!(props.Flip & TextureFlip_X)) * 2;

			s_Data->QuadBufferPtr->a_TexRegion.y += (props.Flip & TextureFlip_Y) * s_Data->QuadBufferPtr->a_TexRegion.w;
			s_Data->QuadBufferPtr->a_TexRegion.w *= -1 + (int)(!(props.Flip & TextureFlip_Y)) * 2;
		}
		
		s_Data->QuadBufferPtr->a_TexCoord.x *= props.Tiling.x;
		s_Data->QuadBufferPtr->a_TexCoord.y *= props.Tiling.y;
		s_Data->QuadBufferPtr->a_TexCoord.z *= props.Tiling.x;
		s_Data->QuadBufferPtr->a_TexCoord.w *= props.Tiling.y;

		s_Data->QuadBufferPtr->a_TexCoord.x += props.Offset.x;
		s_Data->QuadBufferPtr->a_TexCoord.y += props.Offset.y;

		s_Data->QuadBufferPtr->a_TexCoord.x += (props.Flip & TextureFlip_X) * s_Data->QuadBufferPtr->a_TexCoord.z;
		s_Data->QuadBufferPtr->a_TexCoord.z *= -1 + (int)(!(props.Flip & TextureFlip_X)) * 2;

		s_Data->QuadBufferPtr->a_TexCoord.y += (props.Flip & TextureFlip_X) * s_Data->QuadBufferPtr->a_TexCoord.w;
		s_Data->QuadBufferPtr->a_TexCoord.w *= -1 + (int)(!(props.Flip & TextureFlip_Y)) * 2;

		s_Data->QuadBufferPtr++;
		s_Data->QuadCount++;
		s_Statistics.QuadCount++;
	}
}
