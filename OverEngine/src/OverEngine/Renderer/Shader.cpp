#include "pcheader.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace OverEngine
{

	OverEngine::Shader* Shader::Create(String& vertexSrc, String& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::OpenGL:  return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}