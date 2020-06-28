#include "pcheader.h"
#include "OpenGLRendererAPI.h"

#include <glad/gl.h>

namespace OverEngine
{
	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GLint texture_units;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
		OE_CORE_INFO("Maximum Texture Slots : {0}", texture_units);

		GLint max_texture_size;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
		OE_CORE_INFO("Maximum Texture Size : {0}", max_texture_size);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

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