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
		glm::ivec2 a_TexWrap = glm::ivec2(0);
		Vector2    a_TexSize     = Vector2(0.0f);
		Rect       a_TexRect     = Rect(0.0f);
	};

	// Limits
	static constexpr uint32_t MaxTextureCount = 32;
	static const uint32_t MaxQuadCount = 1000000;
	
	struct Renderer2DData
	{
		Ref<VertexArray>  QuadVA = nullptr;
		Ref<VertexBuffer> QuadVB = nullptr;

		Vertex* QuadBufferBasePtr = nullptr;
		Vertex* QuadBufferPtr = nullptr;

		Vector<float> QuadsZIndices;
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

			{ ShaderDataType::Float2, "a_TexTiling" },
			{ ShaderDataType::Float2, "a_TexOffset" },
			{ ShaderDataType::Int, "a_TexFlip" },
			{ ShaderDataType::Int, "a_TexSlot" },
			{ ShaderDataType::Int, "a_TexFilter" },
			{ ShaderDataType::Int2, "a_TexWrap" },
			{ ShaderDataType::Float2, "a_TexSize" },
			{ ShaderDataType::Float4, "a_TexRect" },
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
		s_Data->QuadsZIndices.reserve(MaxQuadCount);

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
		s_Data->QuadsZIndices.clear();
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
		s_Data->QuadsZIndices.clear();
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

		if (s_Data->QuadCount + 1 >= MaxQuadCount)
		{
			NextBatch();
		}

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

	/////////////////////////////////////////////////////////
	// Textured Quad ////////////////////////////////////////
	/////////////////////////////////////////////////////////

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
		if (!props.Texture)
			return;

		if (props.Tint.a == 0)
			return;

		if (s_Data->QuadCount + 1 >= MaxQuadCount)
		{
			NextBatch();
		}

		int8_t textureSlot = -1;
		{
			Ref<Texture2D> gpuTex = props.Texture;
			if (props.Texture->GetType() == TextureType::SubTexture)
				gpuTex = std::dynamic_pointer_cast<SubTexture2D>(props.Texture)->GetMasterTexture();

			auto end = s_Data->TextureBindList.begin() + s_Data->TextureCount;
			auto it = std::find(s_Data->TextureBindList.begin(), end, gpuTex);
			if (it == end)
			{
				uint8_t slot = s_Data->TextureCount;
				if (slot + 1 > RenderCommand::GetMaxTextureSlotCount())
				{
					NextBatch();
					slot = 0;
				}
				s_Data->TextureBindList[slot] = gpuTex;
				s_Data->TextureCount++;
				textureSlot = slot;
			}
			else
			{
				textureSlot = it - s_Data->TextureBindList.begin();
			}
		}

		auto mat = s_Data->ViewProjectionMatrix * transform;

		s_Data->QuadBufferPtr->a_Position0 = Vector3(mat * Vector4(-0.5, -0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr->a_Position1 = Vector3(mat * Vector4( 0.5, -0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr->a_Position2 = Vector3(mat * Vector4(-0.5,  0.5, 0.0, 1.0));
		s_Data->QuadBufferPtr->a_Position3 = Vector3(mat * Vector4( 0.5,  0.5, 0.0, 1.0));

		s_Data->QuadBufferPtr->a_Color = props.Tint;

		s_Data->QuadBufferPtr->a_TexTiling = props.Tiling;
		s_Data->QuadBufferPtr->a_TexOffset = props.Offset;
		s_Data->QuadBufferPtr->a_TexFlip = props.Flip;
		s_Data->QuadBufferPtr->a_TexSlot = textureSlot;
		
		// a_TexFilter
		if (props.Filter != TextureFilter::None)
			s_Data->QuadBufferPtr->a_TexFilter = (int)props.Filter;
		else
			s_Data->QuadBufferPtr->a_TexFilter = (int)props.Texture->GetFilter();

		// a_TexUWrap & a_TexVWrap
		if (props.Wrap.x != TextureWrap::None)
			s_Data->QuadBufferPtr->a_TexWrap.x = (int)props.Wrap.x;
		else
			s_Data->QuadBufferPtr->a_TexWrap.x = (int)props.Texture->GetUWrap();

		if (props.Wrap.y != TextureWrap::None)
			s_Data->QuadBufferPtr->a_TexWrap.y = (int)props.Wrap.y;
		else
			s_Data->QuadBufferPtr->a_TexWrap.y = (int)props.Texture->GetVWrap();

		s_Data->QuadBufferPtr->a_TexSize = { props.Texture->GetWidth(), props.Texture->GetHeight() };

		if (props.Texture->GetType() == TextureType::Master)
			s_Data->QuadBufferPtr->a_TexRect = { 0, 0, 1, 1 };
		else
			s_Data->QuadBufferPtr->a_TexRect = std::dynamic_pointer_cast<SubTexture2D>(props.Texture)->GetRect();

		s_Data->QuadBufferPtr++;
		s_Data->QuadCount++;
		s_Statistics.QuadCount++;
	}
}
