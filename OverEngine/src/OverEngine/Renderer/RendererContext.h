#pragma once

namespace OverEngine
{
	class Window;

	class RendererContext
	{
	public:
		virtual ~RendererContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		virtual void Current() = 0;

		virtual const char* GetInfoVersion() = 0;
		virtual const char* GetInfoVendor() = 0;
		virtual const char* GetInfoRenderer() = 0;

		static Scope<RendererContext> Create(Window* window);
	};
}
