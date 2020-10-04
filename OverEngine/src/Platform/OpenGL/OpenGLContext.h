#pragma once

#include "OverEngine/Renderer/RendererContext.h"

struct GLFWwindow;

namespace OverEngine
{
	class OpenGLContext : public RendererContext
	{
	public:
		OpenGLContext(Window* window);

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual void Current() override;

		virtual const char* GetInfoVersion()  override;
		virtual const char* GetInfoVendor()   override;
		virtual const char* GetInfoRenderer() override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}
