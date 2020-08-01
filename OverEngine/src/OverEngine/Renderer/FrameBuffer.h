#pragma once

#include "OverEngine/Core/Core.h"

namespace OverEngine
{
	struct FrameBufferProps
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;

		virtual const FrameBufferProps& GetProps() const = 0;

		static Ref<FrameBuffer> Create(const FrameBufferProps& props);
	};
}