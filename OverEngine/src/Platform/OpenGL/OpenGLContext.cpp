#include "pcheader.h"
#include "OpenGLContext.h"

#include "OverEngine/Core/Window.h"

#include <GLFW/glfw3.h>
#include <glad/gl.h>

namespace OverEngine
{
	unsigned int OpenGLContext::s_ContextCount = 0;

	OpenGLContext::OpenGLContext(Window* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle->GetNativeWindow())),
		m_GenericWindowHandle(windowHandle)
	{
		OE_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		if (s_ContextCount == 0)
		{
			int status = gladLoadGL(glfwGetProcAddress);
			OE_CORE_ASSERT(status, "Failed to initialize Glad!");
		}
		s_ContextCount++;

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
		if (m_GenericWindowHandle->IsDoubleBuffered())
			glfwSwapBuffers(m_WindowHandle);
		else
			glFlush();
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