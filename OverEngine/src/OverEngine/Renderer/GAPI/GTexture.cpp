#include "pcheader.h"
#include "GTexture.h"

#include "OverEngine/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace OverEngine
{
	namespace GAPI
	{
		Ref<Texture2D> Texture2D::Create(const String& path, TextureFiltering minFilter /*= Filtering::Linear*/, TextureFiltering magFilter /*= Filtering::Linear*/)
		{
			switch (RendererAPI::GetAPI())
			{
			case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path, minFilter, magFilter);
			}

			OE_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}

		Ref<Texture2D> Texture2D::Create()
		{
			switch (RendererAPI::GetAPI())
			{
			case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>();
			}

			OE_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}
	}
}
