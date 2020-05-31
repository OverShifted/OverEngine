#include "pcheader.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace OverEngine
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}