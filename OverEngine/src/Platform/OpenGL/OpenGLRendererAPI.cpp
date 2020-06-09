#include "pcheader.h"
#include "OpenGLRendererAPI.h"

#include <glad/gl.h>

namespace OverEngine
{
	namespace Renderer
	{
		void OpenGLRendererAPI::SetClearColor(const Color& color)
		{
			glClearColor(color.r, color.g, color.b, color.a);
		}

		void OpenGLRendererAPI::SetClearDepth(float depth)
		{
			glClearDepth(depth);
		}

		void OpenGLRendererAPI::Clear(bool clearColorBuffer, bool clearDepthBuffer)
		{
			uint32_t mask = 0;
			if (clearColorBuffer)
				mask |= GL_COLOR_BUFFER_BIT;
			if (clearDepthBuffer)
				mask |= GL_DEPTH_BUFFER_BIT;
			glClear(mask);
		}

		void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		}
	}
}