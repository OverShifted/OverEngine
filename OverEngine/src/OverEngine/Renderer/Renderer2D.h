#pragma once

#include "RenderCommand.h"

#include "OverEngine/Renderer/Shader.h"
#include "OverEngine/Renderer/Camera.h"
#include "OverEngine/Renderer/Texture.h"

namespace OverEngine
{
	struct TexturedQuadProps
	{
		Color Tint = Color(1.0f);
		Ref<Texture2D> Texture;

		Vector2 Tiling = Vector2(1.0f);
		Vector2 Offset = Vector2(0.0f);
		uint8_t Flip = 0;

		enum Flip_ : uint8_t
		{
			Flip_None = 0,
			Flip_X = BIT(0),
			Flip_Y = BIT(1),
			Flip_Both = Flip_X | Flip_Y
		};

		Vec2T<TextureWrapping> Wrapping{ TextureWrapping::None, TextureWrapping::None };
		TextureFiltering Filtering = TextureFiltering::None;
	};

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void Reset();

		static void BeginScene(const Mat4x4& viewMatrix, const Camera& camera);
		static void EndScene();

		static void StartBatch();
		static void NextBatch();
		static void Flush();

		static void DrawQuad(const Vector2& position, float rotation, const Vector2& size, const Color& color);
		static void DrawQuad(const Vector3& position, float rotation, const Vector2& size, const Color& color);
		static void DrawQuad(const Mat4x4& transform, const Color& color);

		static void DrawQuad(const Vector2& position, float rotation, const Vector2& size, const TexturedQuadProps& props = TexturedQuadProps());
		static void DrawQuad(const Vector3& position, float rotation, const Vector2& size, const TexturedQuadProps& props = TexturedQuadProps());
		static void DrawQuad(const Mat4x4& transform, const TexturedQuadProps& props = TexturedQuadProps());

		struct Statistics
		{
			void Reset()
			{
				QuadCount = 0;
				DrawCalls = 0;
			}

			uint32_t GetIndexCount() { return QuadCount; }
			uint32_t GetVertexCount() { return QuadCount; }

			uint32_t QuadCount;
			uint32_t DrawCalls;
		};

		static Statistics& GetStatistics() { return s_Statistics; }
		static Ref<Shader>& GetShader();
	private:
		static Statistics s_Statistics;
	};
}
