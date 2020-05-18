#include "pcheader.h"
#include "RendererContext.h"

#include "Renderer.h"

#include "OverEngine/Core/Window.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace OverEngine
{
	RendererContext* RendererContext::Create(Window* handle)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::OpenGL:  return new OpenGLContext(handle);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}