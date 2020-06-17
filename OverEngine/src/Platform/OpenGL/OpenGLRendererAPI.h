#pragma once

#include "OverEngine/Renderer/RendererAPI.h"

namespace OverEngine
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void SetClearColor(const Color& color) override;
		virtual void SetClearDepth(float depth) override;
		virtual void Clear(bool clearColorBuffer, bool clearDepthBuffer) override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	};
}