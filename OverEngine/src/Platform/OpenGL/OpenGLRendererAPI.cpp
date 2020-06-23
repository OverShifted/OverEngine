#include "pcheader.h"
#include "OpenGLRendererAPI.h"

#include <glad/gl.h>

namespace OverEngine
{
	void OpenGLRendererAPI::SetClearColor(const Math::Color& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::SetClearDepth(float depth)
	{
		glClearDepth(depth);
	}

	void OpenGLRendererAPI::Clear(bool clearColorBuffer, bool clearDepthBuffer)
	{
		GLbitfield mask = 0;
		if (clearColorBuffer)
			mask |= GL_COLOR_BUFFER_BIT;
		if (clearDepthBuffer)
			mask |= GL_DEPTH_BUFFER_BIT;
		glClear(mask);
	}

	void OpenGLRendererAPI::DrawIndexed(const VertexArray& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}