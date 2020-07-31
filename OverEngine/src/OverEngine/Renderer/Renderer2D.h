#pragma once

#include "RenderCommand.h"

#include "OverEngine/Renderer/Shader.h"
#include "OverEngine/Renderer/Camera.h"
#include "OverEngine/Renderer/Texture.h"

namespace OverEngine
{
	struct TexturedQuadExtraData
	{
		Color tint = Color(1.0f);
		float tilingFactorX = 1.0f;
		float tilingFactorY = 1.0f;
		bool flipX = false;
		bool flipY = false;
		TextureWrapping overrideSTextureWrapping = TextureWrapping::None;
		TextureWrapping overrideTTextureWrapping = TextureWrapping::None;
	};

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera);
		static void EndScene();

		static void DrawQuad(const Vector2& position, float rotation, const Vector2& size, const Color& color);
		static void DrawQuad(const Vector3& position, float rotation, const Vector2& size, const Color& color);
		static void DrawQuad(const Mat4x4& transform, const Color& color);

		static void DrawQuad(const Vector2& position, float rotation, const Vector2& size, Ref<Texture2D> texture, const TexturedQuadExtraData& extraData = TexturedQuadExtraData());
		static void DrawQuad(const Vector3& position, float rotation, const Vector2& size, Ref<Texture2D> texture, const TexturedQuadExtraData& extraData = TexturedQuadExtraData());
		static void DrawQuad(const Mat4x4& transform, Ref<Texture2D> texture, const TexturedQuadExtraData& extraData = TexturedQuadExtraData());

		struct Statistics
		{
			void Reset()
			{
				QuadCount = 0;
				DrawCalls = 0;
			}

			uint32_t GetIndexCount() { return 6 * QuadCount; }
			uint32_t GetVertexCount() { return 4 * QuadCount; }

			uint32_t QuadCount;
			uint32_t DrawCalls;
		};

		static Statistics& GetStatistics() { return s_Statistics; }
	private:
		static Statistics s_Statistics;
	};
}