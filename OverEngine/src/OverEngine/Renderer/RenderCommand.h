#pragma once

#include "RendererAPI.h"

namespace OverEngine
{
	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const Math::Color& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void SetClearDepth(float depth)
		{
			s_RendererAPI->SetClearDepth(depth);
		}

		inline static void Clear(bool clearColorBuffer = true, bool clearDepthBuffer = true)
		{
			s_RendererAPI->Clear(clearColorBuffer, clearDepthBuffer);
		}

		inline static void DrawIndexed(const VertexArray& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}