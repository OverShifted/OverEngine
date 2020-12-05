#include "pcheader.h"
#include "OpenGLRendererAPI.h"

#include <glad/gl.h>

namespace OverEngine
{
	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		// Suppress some useless warnings
		switch (id)
		{
		// NVIDIA: "shader will be recompiled due to GL state mismatches"
		case 131218: return;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         OE_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       OE_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          OE_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: OE_CORE_TRACE(message); return;
		}

		OE_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
	#ifdef OE_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	#endif

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

	void OpenGLRendererAPI::Clear(const ClearFlags& flags)
	{
		GLbitfield mask = 0;
		if (flags & ClearFlags_ClearColor)
			mask |= GL_COLOR_BUFFER_BIT;
		if (flags & ClearFlags_ClearDepth)
			mask |= GL_DEPTH_BUFFER_BIT;
		glClear(mask);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount, DrawType drawType)
	{
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();

		GLenum mode;
		if (drawType == DrawType::Points)    mode = GL_POINTS;
		if (drawType == DrawType::Lines)     mode = GL_LINES;
		if (drawType == DrawType::Triangles) mode = GL_TRIANGLES;

		glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
	}

	uint32_t OpenGLRendererAPI::GetMaxTextureSize()
	{
		GLint max_texture_size;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
		return max_texture_size;
	}

	uint32_t OpenGLRendererAPI::GetMaxTextureSlotCount()
	{
		GLint texture_units;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
		return texture_units;
	}
}
