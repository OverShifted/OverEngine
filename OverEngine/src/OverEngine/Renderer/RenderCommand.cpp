#include "pcheader.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace OverEngine
{
	static RendererAPI* CreateRendererAPI()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return new OpenGLRendererAPI();
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	RendererAPI* RenderCommand::s_RendererAPI = CreateRendererAPI();
}