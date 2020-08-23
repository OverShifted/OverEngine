#include "pcheader.h"
#include "Renderer2D.h"

#include "RendererAPi.h"

#include "Texture.h"

namespace OverEngine
{
	Renderer2D::Statistics Renderer2D::s_Statistics;

	struct Renderer2DData
	{
		Ref<VertexArray> vertexArray = nullptr;
		Ref<VertexBuffer> vertexBuffer = nullptr;
		Ref<IndexBuffer> indexBuffer = nullptr;

		Ref<Shader> BatchRenderer2DShader = nullptr;

		Vector<float> Vertices;
		Vector<uint32_t> Indices;
		Mat4x4 ViewProjectionMatrix;
		uint32_t QuadCount = 0;

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

		constexpr char* BatchRenderer2DVertexShaderSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec4 a_Position;
			layout(location = 1) in vec4 a_Color;
			layout(location = 2) in float a_UseTexture;
			layout(location = 3) in float a_TextureSWrapping;
			layout(location = 4) in float a_TextureTWrapping;
			layout(location = 5) in float a_TextureFilter;
			layout(location = 6) in float a_TextureSlot;
			layout(location = 7) in float a_TextureFlipX;
			layout(location = 8) in float a_TextureFlipY;
			layout(location = 9)  in float a_TextureTilingFactorX;
			layout(location = 10) in float a_TextureTilingFactorY;
			layout(location = 11) in vec4 a_TextureBorderColor;
			layout(location = 12) in vec4 a_TextureRect;
			layout(location = 13) in vec2 a_TextureSize;
			layout(location = 14) in vec2 a_TextureCoord;
			
			out vec4 v_Color;
			flat out int v_UseTexture;
			flat out int v_TextureSWrapping;
			flat out int v_TextureTWrapping;
			flat out int v_TextureFilter;
			flat out int v_TextureSlot;
			flat out int v_TextureFlipX;
			flat out int v_TextureFlipY;
			out float v_TextureTilingFactorX;
			out float v_TextureTilingFactorY;
			out vec4 v_TextureBorderColor;
			out vec4 v_TextureRect;
			out vec2 v_TextureSize;
			out vec2 v_TextureCoord;
			
			void main()
			{
				gl_Position = a_Position;
				v_Color = a_Color;
				v_UseTexture = int(a_UseTexture);
				v_TextureSWrapping = int(a_TextureSWrapping);
				v_TextureTWrapping = int(a_TextureTWrapping);
				v_TextureFilter = int(a_TextureFilter);
				v_TextureSlot = int(a_TextureSlot);
				v_TextureFlipX = int(a_TextureFlipX);
				v_TextureFlipY = int(a_TextureFlipY);
				v_TextureTilingFactorX = a_TextureTilingFactorX;
				v_TextureTilingFactorY = a_TextureTilingFactorY;
				v_TextureBorderColor = a_TextureBorderColor;
				v_TextureRect = a_TextureRect;
				v_TextureSize = a_TextureSize;
				v_TextureCoord = a_TextureCoord;
			}
		)";

		constexpr char* BatchRenderer2DFragmentShaderSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 o_Color;
			
			uniform sampler2D[32] u_Slots;
			
			in vec4 v_Color;
			flat in int v_UseTexture;
			flat in int v_TextureSWrapping;
			flat in int v_TextureTWrapping;
			flat in int v_TextureFilter;
			flat in int v_TextureSlot;
			flat in int v_TextureFlipX;
			flat in int v_TextureFlipY;
			in float v_TextureTilingFactorX;
			in float v_TextureTilingFactorY;
			in vec4 v_TextureBorderColor;
			in vec4 v_TextureRect;
			in vec2 v_TextureSize;
			in vec2 v_TextureCoord;
			
			vec4 PointSampleFromAtlas(sampler2D slot, vec2 coord, vec4 subTextureRect, int SWrapping, int TWrapping);
			
			vec4 BiLinearSampleFromAtlas(sampler2D slot, vec2 texCoord)
			{
				texCoord.x *= v_TextureRect.z * v_TextureSize.x;
				texCoord.y *= v_TextureRect.w * v_TextureSize.y;
			
				float xmin = texCoord.x;
				float xmax = xmin + 1.0;
			
				float ymin = texCoord.y;
				float ymax = ymin + 1.0;
			
				vec2 crd1 = vec2(xmin / v_TextureSize.x, ymin / v_TextureSize.y);
				vec2 crd2 = vec2(xmax / v_TextureSize.x, ymin / v_TextureSize.y);
			
				vec4 TopMix = mix(
					PointSampleFromAtlas(slot, crd1 / v_TextureRect.zw, v_TextureRect, 1, 1),
					PointSampleFromAtlas(slot, crd2 / v_TextureRect.zw, v_TextureRect, 1, 1),
					fract(texCoord.x)
				);
			
				crd1 = vec2(xmin / v_TextureSize.x, ymax / v_TextureSize.y);
				crd2 = vec2(xmax / v_TextureSize.x, ymax / v_TextureSize.y);
				vec4 DownMix = mix(
					PointSampleFromAtlas(slot, crd1 / v_TextureRect.zw, v_TextureRect, 1, 1),
					PointSampleFromAtlas(slot, crd2 / v_TextureRect.zw, v_TextureRect, 1, 1),
					fract(texCoord.x)
				);
			
				return mix(TopMix, DownMix, fract(texCoord.y));
			}
			
			vec4 PointSampleFromAtlas(sampler2D slot, vec2 coord, vec4 subTextureRect, int SWrapping, int TWrapping)
			{
				coord.y = 1 - coord.y;
			
				switch (SWrapping)
				{
				case 0: // Repeat
					if (coord.x > 1)
						coord.x -= coord.x == int(coord.x) ? int(coord.x) - 1 : int(coord.x);
			
					else if (coord.x < 0)
						coord.x -= coord.x == int(coord.x) ? int(coord.x) - 2 : int(coord.x) - 1;
					break;
				case 1: // MirroredRepeat
					if (coord.x > 1)
					{
						if (int(coord.x) % 2 == 0)
							coord.x -= coord.x == int(coord.x) ? int(coord.x) - 1 : int(coord.x);
						else
							coord.x = (coord.x == int(coord.x) ? int(coord.x) : int(coord.x) + 1) - coord.x;
					}
			
					else if (coord.x < 0)
					{
						if (int(coord.x) % 2 == 0)
							coord.x = (coord.x == int(coord.x) ? int(coord.x) - 1 : int(coord.x)) - coord.x;
						else
							coord.x -= coord.x == int(coord.x) ? int(coord.x) - 2 : int(coord.x) - 1;
			
					}
					break;
				case 2: // ClampToEdge
					coord.x = clamp(coord.x, 0.001f, 0.999f);
					break;
				case 3: // ClampToBorder
					if (coord.x > 1 || coord.x < 0)
						return v_TextureBorderColor;
					break;
				}
			
				switch (TWrapping)
				{
				case 0: // Repeat
					if (coord.y > 1)
						coord.y -= coord.y == int(coord.y) ? int(coord.y) - 1 : int(coord.y);
			
					else if (coord.y < 0)
						coord.y -= coord.y == int(coord.y) ? int(coord.y) - 2 : int(coord.y) - 1;
					break;
				case 1: // MirroredRepeat
					if (coord.y > 1)
					{
						if (int(coord.y) % 2 == 0)
							coord.y -= coord.y == int(coord.y) ? int(coord.y) - 1 : int(coord.y);
						else
							coord.y = (coord.y == int(coord.y) ? int(coord.y) : int(coord.y) + 1) - coord.y;
					}
				
					else if (coord.y < 0)
					{
						if (int(coord.y) % 2 == 0)
							coord.y = (coord.y == int(coord.y) ? int(coord.y) - 1 : int(coord.y)) - coord.y;
						else
							coord.y -= coord.y == int(coord.y) ? int(coord.y) - 2 : int(coord.y) - 1;
					}
					break;
				case 2: // ClampToEdge
					coord.y = clamp(coord.y, 0.0f, 1.0f);
					break;
				case 3: // ClampToBorder
					if (coord.y > 1 || coord.y < 0)
						return v_TextureBorderColor;
					break;
				}
			
				return texture(slot, vec2(subTextureRect.x + coord.x * subTextureRect.z, subTextureRect.y + coord.y * subTextureRect.w));
			}
			
			vec4 Sample(sampler2D slot, vec2 coord)
			{
				if (v_TextureFilter == 0)
						return PointSampleFromAtlas(slot, coord * vec2(v_TextureTilingFactorX, v_TextureTilingFactorY), v_TextureRect, v_TextureSWrapping, v_TextureTWrapping) * v_Color;
				return BiLinearSampleFromAtlas(slot, coord * vec2(v_TextureTilingFactorX, v_TextureTilingFactorY)) * v_Color;
			}
			
			void main()
			{
				vec2 coord = v_TextureCoord;
			
				if (v_TextureFlipX != 0)
					coord.x = 1 - coord.x;
			
				if (v_TextureFlipY != 0)
					coord.y = 1 - coord.y;
			
				if (v_UseTexture != 0)
				{
					switch (v_TextureSlot)
					{
						case 0: o_Color = Sample(u_Slots[0], coord); break;
						case 1: o_Color = Sample(u_Slots[1], coord); break;
						case 2: o_Color = Sample(u_Slots[2], coord); break;
						case 3: o_Color = Sample(u_Slots[3], coord); break;
						case 4: o_Color = Sample(u_Slots[4], coord); break;
						case 5: o_Color = Sample(u_Slots[5], coord); break;
						case 6: o_Color = Sample(u_Slots[6], coord); break;
						case 7: o_Color = Sample(u_Slots[7], coord); break;
						case 8: o_Color = Sample(u_Slots[8], coord); break;
						case 9: o_Color = Sample(u_Slots[9], coord); break;
						case 10: o_Color = Sample(u_Slots[10], coord); break;
						case 11: o_Color = Sample(u_Slots[11], coord); break;
						case 12: o_Color = Sample(u_Slots[12], coord); break;
						case 13: o_Color = Sample(u_Slots[13], coord); break;
						case 14: o_Color = Sample(u_Slots[14], coord); break;
						case 15: o_Color = Sample(u_Slots[15], coord); break;
						case 16: o_Color = Sample(u_Slots[16], coord); break;
						case 17: o_Color = Sample(u_Slots[17], coord); break;
						case 18: o_Color = Sample(u_Slots[18], coord); break;
						case 19: o_Color = Sample(u_Slots[19], coord); break;
						case 20: o_Color = Sample(u_Slots[20], coord); break;
						case 21: o_Color = Sample(u_Slots[21], coord); break;
						case 22: o_Color = Sample(u_Slots[22], coord); break;
						case 23: o_Color = Sample(u_Slots[23], coord); break;
						case 24: o_Color = Sample(u_Slots[24], coord); break;
						case 25: o_Color = Sample(u_Slots[25], coord); break;
						case 26: o_Color = Sample(u_Slots[26], coord); break;
						case 27: o_Color = Sample(u_Slots[27], coord); break;
						case 28: o_Color = Sample(u_Slots[28], coord); break;
						case 29: o_Color = Sample(u_Slots[29], coord); break;
						case 30: o_Color = Sample(u_Slots[30], coord); break;
						case 31: o_Color = Sample(u_Slots[31], coord); break;
					}
				}
				else
				{
					o_Color = v_Color;
				}
			}
		)";

		// s_Data->BatchRenderer2DShader = Shader::Create("assets/shaders/BatchRenderer2D.glsl");
		s_Data->BatchRenderer2DShader = Shader::Create("BatchRenderer2D", BatchRenderer2DVertexShaderSrc, BatchRenderer2DFragmentShaderSrc);
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
		s_Data->vertexBuffer->BufferData(s_Data->Vertices.data(), (uint32_t)(s_Data->Vertices.size() * sizeof(float)), true);
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

			vertexPosition.x = Renderer2DData::QuadVertices[0 + 3 * i];
			vertexPosition.y = Renderer2DData::QuadVertices[1 + 3 * i];
			vertexPosition.z = Renderer2DData::QuadVertices[2 + 3 * i];
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

			for (uint32_t i = 0; i < 21; i++)
				s_Data->Vertices.push_back(0.0f);
		}

		for (uint32_t idx : Renderer2DData::QuadIndices)
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

			vertexPosition.x = Renderer2DData::QuadVertices[0 + 3 * i];
			vertexPosition.y = Renderer2DData::QuadVertices[1 + 3 * i];
			vertexPosition.z = Renderer2DData::QuadVertices[2 + 3 * i];
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
			s_Data->Vertices.push_back(Renderer2DData::QuadVertices[0 + 3 * i] > 0.0f ? 1.0f : 0.0f);
			s_Data->Vertices.push_back(Renderer2DData::QuadVertices[1 + 3 * i] > 0.0f ? 1.0f : 0.0f);
		}

		for (uint32_t idx : Renderer2DData::QuadIndices)
			s_Data->Indices.push_back((uint32_t)(idx + 4 * s_Statistics.QuadCount));

		s_Statistics.QuadCount++;
	}
}