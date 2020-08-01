#include "pcheader.h"
#include "FrameBuffer.h"

#include "OverEngine/Core/Core.h"

#include "OverEngine/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace OverEngine
{

	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferProps& props)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFrameBuffer>(props);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}