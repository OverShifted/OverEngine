#include "pcheader.h"
#include "RendererContext.h"

#include "RendererAPI.h"

#include "OverEngine/Core/Window.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace OverEngine
{
	namespace Renderer
	{
		RendererContext* RendererContext::Create(Window* handle)
		{
			switch (RendererAPI::GetAPI())
			{
			case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return new OpenGLContext(handle);
			}

			OE_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}
	}
}