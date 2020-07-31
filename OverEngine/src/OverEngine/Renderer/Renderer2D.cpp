#include "pcheader.h"
#include "Renderer2D.h"

#include "Texture.h"

namespace OverEngine
{
	Renderer2D::Statistics Renderer2D::s_Statistics;

	struct Renderer2DData
	{
		Ref<VertexArray> vertexArray;
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;

		Ref<Shader> BatchRenderer2DShader;

		Vector<float> Vertices;
		Vector<uint32_t> Indices;
		Mat4x4 ViewProjectionMatrix;
		uint32_t QuadCount;

		float QuadVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		uint32_t QuadIndices[6] = { 0, 1, 2, 2, 3, 0 };

		int ShaderSampler2Ds[32] = {
			0, 1, 2, 3, 4, 5, 6, 7,
			8, 9, 10, 11, 12, 13, 14, 15,
			16, 17, 18, 19, 20, 21, 22, 23,
			24, 25, 26, 27, 28, 29, 30, 31
		};

		Vector<std::pair<uint32_t, Ref<GAPI::Texture2D>>> TextureBindList;
	};

	static Renderer2DData* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DData();

		s_Data->vertexArray = VertexArray::Create();

		s_Data->vertexBuffer = VertexBuffer::Create();
		s_Data->vertexBuffer->SetLayout({
			{ ShaderDataType::Float4, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },

			{ ShaderDataType::Float, "a_UseTexture" },
			{ ShaderDataType::Float, "a_TextureSWrapping" },
			{ ShaderDataType::Float, "a_TextureTWrapping" },
			{ ShaderDataType::Float, "a_TextureFilter" },
			{ ShaderDataType::Float, "a_TextureSlot" },
			{ ShaderDataType::Float, "a_TextureFlipX" },
			{ ShaderDataType::Float, "a_TextureFlipY" },
			{ ShaderDataType::Float, "a_TextureTilingFactorX" },
			{ ShaderDataType::Float, "a_TextureTilingFactorY" },
			{ ShaderDataType::Float4, "a_TextureBorderColor" },
			{ ShaderDataType::Float4, "a_TextureRect" },
			{ ShaderDataType::Float2, "a_TextureSize" },
			{ ShaderDataType::Float2, "a_TextureCoord" },
		});
		s_Data->vertexArray->AddVertexBuffer(s_Data->vertexBuffer);

		s_Data->indexBuffer = IndexBuffer::Create();
		s_Data->vertexArray->SetIndexBuffer(s_Data->indexBuffer);

		s_Data->BatchRenderer2DShader = Shader::Create("assets/shaders/BatchRenderer2D.glsl");
		s_Data->BatchRenderer2DShader->Bind();
		s_Data->BatchRenderer2DShader->UploadUniformIntArray("u_Slots", s_Data->ShaderSampler2Ds, 32);

		s_Statistics.Reset();
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::BeginScene(const Camera& camera)
	{
		s_Data->Vertices.clear();
		s_Data->Indices.clear();
		
		s_Statistics.Reset();

		s_Data->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer2D::EndScene()
	{
		s_Data->vertexBuffer->BufferData(s_Data->Vertices.data(), (uint32_t)s_Data->Vertices.size() * (uint32_t)sizeof(float), true);
		s_Data->indexBuffer->BufferData(s_Data->Indices.data(), (uint32_t)s_Data->Indices.size(), true);
		
		// Bind Textures
		for (auto& t : s_Data->TextureBindList)
			t.second->Bind(t.first);

		s_Data->vertexArray->Bind();
		RenderCommand::DrawIndexed(*s_Data->vertexArray);

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
		for (int i = 0; i < 4; i++)
		{
			Vector4 vertexPosition;

			vertexPosition.x = s_Data->QuadVertices[0 + 3 * i];
			vertexPosition.y = s_Data->QuadVertices[1 + 3 * i];
			vertexPosition.z = s_Data->QuadVertices[2 + 3 * i];
			vertexPosition.w = 1.0f;
			vertexPosition = s_Data->ViewProjectionMatrix * transform * vertexPosition;

			s_Data->Vertices.push_back(vertexPosition.x);
			s_Data->Vertices.push_back(vertexPosition.y);
			s_Data->Vertices.push_back(vertexPosition.z);
			s_Data->Vertices.push_back(vertexPosition.w);

			s_Data->Vertices.push_back(color.r);
			s_Data->Vertices.push_back(color.g);
			s_Data->Vertices.push_back(color.b);
			s_Data->Vertices.push_back(color.a);

			for (int i = 0; i < 21; i++)
				s_Data->Vertices.push_back(0.0f);
		}

		for (uint32_t idx : s_Data->QuadIndices)
			s_Data->Indices.push_back((uint32_t)(idx + 4 * s_Statistics.QuadCount));

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
		for (int i = 0; i < 4; i++)
		{
			Vector4 vertexPosition;

			vertexPosition.x = s_Data->QuadVertices[0 + 3 * i];
			vertexPosition.y = s_Data->QuadVertices[1 + 3 * i];
			vertexPosition.z = s_Data->QuadVertices[2 + 3 * i];
			vertexPosition.w = 1.0f;
			vertexPosition = s_Data->ViewProjectionMatrix * transform * vertexPosition;

			// a_Position
			s_Data->Vertices.push_back(vertexPosition.x);
			s_Data->Vertices.push_back(vertexPosition.y);
			s_Data->Vertices.push_back(vertexPosition.z);
			s_Data->Vertices.push_back(vertexPosition.w);

			// a_Color
			s_Data->Vertices.push_back(extraData.tint.r);
			s_Data->Vertices.push_back(extraData.tint.g);
			s_Data->Vertices.push_back(extraData.tint.b);
			s_Data->Vertices.push_back(extraData.tint.a);

			// a_UseTexture
			s_Data->Vertices.push_back(1.0f);

			// a_TextureSWrapping & a_TextureTWrapping
			if (extraData.overrideSTextureWrapping != TextureWrapping::None)
				s_Data->Vertices.push_back((float)extraData.overrideSTextureWrapping);
			else
				s_Data->Vertices.push_back((float)texture->GetSWrapping());

			if (extraData.overrideTTextureWrapping != TextureWrapping::None)
				s_Data->Vertices.push_back((float)extraData.overrideTTextureWrapping);
			else
				s_Data->Vertices.push_back((float)texture->GetTWrapping());


			// a_TextureFilter
			s_Data->Vertices.push_back((float)texture->GetFilter());

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
				s_Data->TextureBindList.push_back({ slot, textureToBind });
				textureSlot = slot;
			}

			// a_TextureSlot
			s_Data->Vertices.push_back((float)textureSlot);

			// a_TextureFlipX & a_TextureFlipY
			s_Data->Vertices.push_back((float)extraData.flipX);
			s_Data->Vertices.push_back((float)extraData.flipY);

			// a_TextureTilingFactorX & a_TextureTilingFactorY
			s_Data->Vertices.push_back(extraData.tilingFactorX);
			s_Data->Vertices.push_back(extraData.tilingFactorY);

			// a_TextureBorderColor
			s_Data->Vertices.push_back(texture->GetBorderColor().x);
			s_Data->Vertices.push_back(texture->GetBorderColor().y);
			s_Data->Vertices.push_back(texture->GetBorderColor().z);
			s_Data->Vertices.push_back(texture->GetBorderColor().w);

			// a_TextureRect
			if (texture->GetType() == TextureType::Master)
			{
				s_Data->Vertices.push_back(texture->m_MasterTextureData.m_MappedTextureRect.x / textureToBind->GetWidth());
				s_Data->Vertices.push_back(texture->m_MasterTextureData.m_MappedTextureRect.y / textureToBind->GetHeight());
				s_Data->Vertices.push_back(texture->m_MasterTextureData.m_MappedTextureRect.z / textureToBind->GetWidth());
				s_Data->Vertices.push_back(texture->m_MasterTextureData.m_MappedTextureRect.w / textureToBind->GetHeight());
			}
			else
			{
				Rect& parentRect = texture->m_SubTextureData.m_Parent->m_MasterTextureData.m_MappedTextureRect;
				Rect rect = texture->m_SubTextureData.m_Rect;

				rect.x += parentRect.x;
				rect.y += parentRect.y;

				s_Data->Vertices.push_back(rect.x / textureToBind->GetWidth());
				s_Data->Vertices.push_back(rect.y / textureToBind->GetHeight());
				s_Data->Vertices.push_back(rect.z / textureToBind->GetWidth());
				s_Data->Vertices.push_back(rect.w / textureToBind->GetHeight());
			}

			// a_TextureSize
			s_Data->Vertices.push_back((float)textureToBind->GetWidth());
			s_Data->Vertices.push_back((float)textureToBind->GetHeight());

			// a_TextureCoord
			s_Data->Vertices.push_back(s_Data->QuadVertices[0 + 3 * i] > 0.0f ? 1.0f : 0.0f);
			s_Data->Vertices.push_back(s_Data->QuadVertices[1 + 3 * i] > 0.0f ? 1.0f : 0.0f);
		}

		for (uint32_t idx : s_Data->QuadIndices)
			s_Data->Indices.push_back((uint32_t)(idx + 4 * s_Statistics.QuadCount));

		s_Statistics.QuadCount++;
	}
}