#include "pcheader.h"
#include "OpenGLContext.h"

#include "OverEngine/Core/Window.h"

#include <GLFW/glfw3.h>
#include <glad/gl.h>

namespace OverEngine
{
	OpenGLContext::OpenGLContext(Window* window)
	{
		m_WindowHandle = static_cast<GLFWwindow*>(window->GetNativeWindow());
		OE_CORE_ASSERT(window, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		static bool glLoaded = false;
		if (!glLoaded)
		{
			glfwMakeContextCurrent(m_WindowHandle);
			int status = gladLoadGL(glfwGetProcAddress);
			OE_CORE_ASSERT(status, "Failed to initialize Glad!");
		}

		OE_CORE_INFO("OpenGL info");
		OE_CORE_INFO("    Version  : {0}", GetInfoVersion());
		OE_CORE_INFO("    Vendor   : {0}", GetInfoVendor());
		OE_CORE_INFO("    Renderer : {0}", GetInfoRenderer());

		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
		OE_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "OverEngine requires at least OpenGL version 4.5 but it got version {0}.{1}", versionMajor, versionMinor);
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

	void OpenGLContext::Current()
	{
		glfwMakeContextCurrent(m_WindowHandle);
	}

	const char* OpenGLContext::GetInfoVersion()
	{
		return (const char*)glGetString(GL_VERSION);
	}

	const char* OpenGLContext::GetInfoVendor()
	{
		return (const char*)glGetString(GL_VENDOR);
	}

	const char* OpenGLContext::GetInfoRenderer()
	{
		return (const char*)glGetString(GL_RENDERER);
	}
}
