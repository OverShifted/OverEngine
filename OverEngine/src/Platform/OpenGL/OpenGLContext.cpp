#include "pcheader.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

namespace OverEngine
{

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
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
		glfwSwapBuffers(m_WindowHandle);
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
		return (const char*)glGetString(GL_RENDERER);
	}
}