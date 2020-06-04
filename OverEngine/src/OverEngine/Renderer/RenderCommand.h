#pragma once

#include "RendererAPI.h"

namespace OverEngine
{
	namespace Renderer
	{
		class RenderCommand
		{
		public:
			inline static void SetClearColor(const Color& color)
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

			inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
			{
				s_RendererAPI->DrawIndexed(vertexArray);
			}
		private:
			static RendererAPI* s_RendererAPI;
		};
	}
}