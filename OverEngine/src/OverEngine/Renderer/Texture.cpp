#include "pcheader.h"
#include "Texture.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace OverEngine
{

	Ref<Texture2D> Texture2D::Create(const String& path, Filtering minFilter, Filtering magFilter)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path, minFilter, magFilter);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}