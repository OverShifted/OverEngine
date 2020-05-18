#include "pcheader.h"
#include "OpenGLContext.h"

#include "OverEngine/Core/Window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace OverEngine
{

	OpenGLContext::OpenGLContext(Window* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle->GetNativeWindow())),
		  m_WindowBackup(nullptr),
		  m_GenericWindowHandle(windowHandle)
	{
		OE_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		OE_CORE_ASSERT(status, "Failed to initialize Glad!");
	}

	void OpenGLContext::SwapBuffers()
	{
		if (m_GenericWindowHandle->IsDoubleBuffered())
			glfwSwapBuffers(m_WindowHandle);
		else
			glFlush();
	}

	void OpenGLContext::SetViewport(int width, int height, int x, int y)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLContext::BackupContext()
	{
		m_WindowBackup = glfwGetCurrentContext();
	}

	void OpenGLContext::ApplyBackupedContext()
	{
		glfwMakeContextCurrent(m_WindowBackup);
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
		//return (const char*)glGetString(GL_RENDERER);
		return (const char*)"GeForce RTX 2080 / PCIe / SSE2";
	}
}