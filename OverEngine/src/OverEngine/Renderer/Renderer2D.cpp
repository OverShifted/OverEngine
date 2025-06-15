#include "pcheader.h"
#include "Renderer2D.h"

#include "Texture.h"

namespace OverEngine
{
	Renderer2D::Statistics Renderer2D::s_Statistics;

	struct Vertex
	{
		Vector3 a_Position = Vector3(0.0f);

		Color   a_Color     = Color(1.0f);
		int     a_TexSlot   = -1;
		Vector2 a_TexCoord  = Vector4(0, 0, 1, 1);
		Vector4 a_TexRegion = Vector4(0.0f);
		int     a_TexRepeat = 0;

		Vector3 a_MidPoint  = Vector3(0.0f);
		Vector2 a_QuadNDC2ScreenNDCScale  = Vector3(1.0f);
		int a_LiquidGlass   = 0;
	};

	// Hard-coded Limits
	static constexpr uint32_t MaxTextureCount = 32;
	static constexpr uint32_t MaxQuadCount = 1000;
	
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
		bool DepthSorting;

		// static constexpr float Qua
	};

	static Renderer2DData* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DData();

		s_Data->QuadVA = VertexArray::Create();

		s_Data->QuadVB = VertexBuffer::Create();
		s_Data->QuadVB->Allocate(MaxQuadCount * 4 * sizeof(Vertex));
		s_Data->QuadVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },

			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Int, "a_TexSlot" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float4, "a_TexRegion" },
			{ ShaderDataType::Int, "a_TexRepeat" },

			{ ShaderDataType::Float3, "a_MidPoint" },
			{ ShaderDataType::Float2, "v_QuadNDC2ScreenNDCScale" },
			{ ShaderDataType::Int, "a_LiquidGlass" }
		});
		s_Data->QuadVA->AddVertexBuffer(s_Data->QuadVB);

		// TODO: Dynamically scale
		{
			auto quadIB = IndexBuffer::Create();
			uint32_t* indices = new uint32_t[6 * MaxQuadCount];

			for (uint32_t i = 0; i < MaxQuadCount; i++) {
				indices[6 * i + 0] = 4 * i + 0;
				indices[6 * i + 1] = 4 * i + 1;
				indices[6 * i + 2] = 4 * i + 3;
				indices[6 * i + 3] = 4 * i + 3;
				indices[6 * i + 4] = 4 * i + 2;
				indices[6 * i + 5] = 4 * i + 0;
			}

			quadIB->Allocate(MaxQuadCount, indices);
			delete[] indices;

			s_Data->QuadVA->SetIndexBuffer(quadIB);
		}

		s_Data->QuadBufferBasePtr = new Vertex[4 * MaxQuadCount];
		s_Data->QuadBufferPtr = s_Data->QuadBufferBasePtr;

		s_Data->Shader = Shader::Create("assets/shaders/BatchRenderer2D.glsl");
		InitShader();

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

	void Renderer2D::ReloadShader()
	{
		s_Data->Shader->Reload();
		InitShader();		
	}

	void Renderer2D::InitShader()
	{
		int textureIDs[MaxTextureCount];
		for (int i = 0; i < (int)MaxTextureCount; i++)
			textureIDs[i] = i;

		s_Data->Shader->Bind();
		s_Data->Shader->UploadUniformIntArray("u_Slots", textureIDs, MaxTextureCount);
	}

	void Renderer2D::Reset()
	{
		s_Data->QuadBufferPtr = s_Data->QuadBufferBasePtr;
		s_Data->OpaqueInsertIndex = 0;
		s_Data->QuadCount = 0;
		s_Data->TextureCount = 0;

		s_Statistics.Reset();
	}

	void Renderer2D::BeginScene(const Mat4x4& viewMatrix, const Camera& camera, bool depthSorting)
	{
		s_Data->ViewProjectionMatrix = camera.GetProjection() * viewMatrix;
		s_Data->DepthSorting = depthSorting;

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

		if (s_Data->DepthSorting)
		{
			std::sort(s_Data->QuadBufferBasePtr, s_Data->QuadBufferBasePtr + s_Data->QuadCount, [](const Vertex& a, const Vertex& b)
			{
				return a.a_Position.z > b.a_Position.z;
			});
		}

		// Upload Data
        s_Data->QuadVB->Upload((void*)s_Data->QuadBufferBasePtr, 4 * s_Data->QuadCount * sizeof(Vertex));

		// Bind Textures
		for (uint8_t i = 0; i < s_Data->TextureCount; i++)
			s_Data->TextureBindList[i]->Bind(i);

		// Bind VertexArray & Shader
		s_Data->QuadVA->Bind();
		s_Data->Shader->Bind();

		// DrawCall
		RenderCommand::DrawIndexed(s_Data->QuadVA, 6 * s_Data->QuadCount, DrawType::Triangles);
		s_Statistics.DrawCalls++;
	}

	////////////////////////////////////////////////////////
	/// FlatColor Quad /////////////////////////////////////
	////////////////////////////////////////////////////////

	void Renderer2D::DrawQuad(const Vector2& position, float rotation, const Vector2& size, const Color& color)
	{
		DrawQuad(Vector3(position, 0.0f), rotation, size, color);
	}

	void Renderer2D::DrawQuad(const Vector3& position, float rotation, const Vector2& size, const Color& color, int liquidGlass)
	{
		Mat4x4 transform =
			glm::translate(Mat4x4(1.0f), position) *
			glm::rotate(Mat4x4(1.0f), rotation, Vector3(0, 0, 1)) *
			glm::scale(Mat4x4(1.0f), Vector3(size, 1.0f));

		DrawQuad(transform, color, liquidGlass);
	}

	void Renderer2D::DrawQuad(const Mat4x4& transform, const Color& color, int liquidGlass)
	{
		if (color.a == 0)
			return;

		if (s_Data->QuadCount + 1 >= MaxQuadCount)
			NextBatch();

		auto mat = s_Data->ViewProjectionMatrix * transform;

		// OE_CORE_INFO("Emitting {}: ({}, {}) ({}, {}) ({}, {}) ({}, {})", liquidGlass,
		// 	s_Data->QuadBufferPtr->a_Position0.x, s_Data->QuadBufferPtr->a_Position0.y,
		// 	s_Data->QuadBufferPtr->a_Position1.x, s_Data->QuadBufferPtr->a_Position1.y,
		// 	s_Data->QuadBufferPtr->a_Position2.x, s_Data->QuadBufferPtr->a_Position2.y,
		// 	s_Data->QuadBufferPtr->a_Position3.x, s_Data->QuadBufferPtr->a_Position3.y);
		
		s_Data->QuadBufferPtr[0].a_Position = Vector3(mat * Vector4(-0.5, -0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr[1].a_Position = Vector3(mat * Vector4( 0.5, -0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr[2].a_Position = Vector3(mat * Vector4(-0.5,  0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr[3].a_Position = Vector3(mat * Vector4( 0.5,  0.5, 0.0, 1.0));

		s_Data->QuadBufferPtr[0].a_TexCoord = Vector2(0.0, 0.0);
		s_Data->QuadBufferPtr[1].a_TexCoord = Vector2(1.0, 0.0);
		s_Data->QuadBufferPtr[2].a_TexCoord = Vector2(0.0, 1.0);
		s_Data->QuadBufferPtr[3].a_TexCoord = Vector2(1.0, 1.0);

		Vector3 midPoint = (s_Data->QuadBufferPtr[0].a_Position + s_Data->QuadBufferPtr[3].a_Position) * 0.5f;
		Vector3 quadNDC2ScreenNDCScale = (s_Data->QuadBufferPtr[3].a_Position - s_Data->QuadBufferPtr[0].a_Position) * 0.5f;
		
		for (int i = 0; i < 4; i++) {
			s_Data->QuadBufferPtr[i].a_Color = color;
			s_Data->QuadBufferPtr[i].a_TexSlot = -1;

			s_Data->QuadBufferPtr[i].a_MidPoint = midPoint;
			s_Data->QuadBufferPtr[i].a_QuadNDC2ScreenNDCScale = quadNDC2ScreenNDCScale;
			s_Data->QuadBufferPtr[i].a_LiquidGlass = liquidGlass;
		}

		s_Data->QuadBufferPtr += 4;
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

		uint8_t slot;
		{
			Ref<Texture2D> gpuTex = (props.Sprite->GetType() == TextureType::SubTexture) ? std::dynamic_pointer_cast<SubTexture2D>(props.Sprite)->GetMasterTexture() : props.Sprite;

			auto end = s_Data->TextureBindList.begin() + s_Data->TextureCount;
			auto it = std::find(s_Data->TextureBindList.begin(), end, gpuTex);
			if (it == end)
			{
				slot = s_Data->TextureCount;
				if (slot + 1u > MaxTextureCount)
				{
					NextBatch();
					slot = 0;
				}
				s_Data->TextureBindList[slot] = gpuTex;
				s_Data->TextureCount++;
			}
			else
			{
				slot = static_cast<int>(it - s_Data->TextureBindList.begin());
			}
		}

		auto mat = s_Data->ViewProjectionMatrix * transform;

		s_Data->QuadBufferPtr[0].a_Position = Vector3(mat * Vector4(-0.5, -0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr[1].a_Position = Vector3(mat * Vector4( 0.5, -0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr[2].a_Position = Vector3(mat * Vector4(-0.5,  0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr[3].a_Position = Vector3(mat * Vector4( 0.5,  0.5, 0.0, 1.0));

		Vector4 texCoord = (props.Sprite->GetType() == TextureType::Master)
								? Vector4(0, 0, 1, 1)
								: std::dynamic_pointer_cast<SubTexture2D>(props.Sprite)->GetRect();

		s_Data->QuadBufferPtr[0].a_TexCoord = Vector2(texCoord.x, texCoord.y) + Vector2(0.0, texCoord.w);
		s_Data->QuadBufferPtr[1].a_TexCoord = Vector2(texCoord.x, texCoord.y) + Vector2(texCoord.z, texCoord.w);
		s_Data->QuadBufferPtr[2].a_TexCoord = Vector2(texCoord.x, texCoord.y);
		s_Data->QuadBufferPtr[3].a_TexCoord = Vector2(texCoord.x, texCoord.y) + Vector2(texCoord.z, 0.0);

		for (int i = 0; i < 4; i++) {
			s_Data->QuadBufferPtr[i].a_Color = props.Tint;
			s_Data->QuadBufferPtr[i].a_TexSlot = slot;
			s_Data->QuadBufferPtr[i].a_TexRepeat = props.ForceTile;
			// s_Data->QuadBufferPtr[i].a_TexRegion = s_Data->QuadBufferPtr->a_TexCoord;

			s_Data->QuadBufferPtr[i].a_LiquidGlass = 0;
		}

		/*

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

		*/

		s_Data->QuadBufferPtr += 4;
		s_Data->QuadCount++;
		s_Statistics.QuadCount++;
	}
}
