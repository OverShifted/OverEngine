#pragma once

#include "OverEngine/Renderer/RendererContext.h"

struct GLFWwindow;

namespace OverEngine
{
	namespace Renderer
	{
		class OpenGLContext : public RendererContext
		{
		public:
			OpenGLContext(Window* windowHandle);

			virtual void Init() override;
			virtual void SwapBuffers() override;
			virtual void SetViewport(int width, int height, int x = 0, int y = 0) override;

			virtual void Current() override;

			virtual const char* GetInfoVersion()  override;
			virtual const char* GetInfoVendor()   override;
			virtual const char* GetInfoRenderer() override;
		private:
			Window* m_GenericWindowHandle;
			GLFWwindow* m_WindowHandle;
		};
	}
}