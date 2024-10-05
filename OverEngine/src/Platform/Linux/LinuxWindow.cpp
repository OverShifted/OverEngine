#include "pcheader.h"
#include "LinuxWindow.h"

#include "OverEngine/Events/ApplicationEvent.h"
#include "OverEngine/Events/MouseEvent.h"
#include "OverEngine/Events/KeyEvent.h"

#include "OverEngine/Core/Runtime/Application.h"

#include "OverEngine/Renderer/GraphicsContext.h"
#include "OverEngine/Renderer/RendererAPI.h"

namespace OverEngine
{
	uint32_t LinuxWindow::s_WindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		OE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<LinuxWindow>(props);
	}

	LinuxWindow::LinuxWindow(const WindowProps& props)
	{
		Init(props);
	}

	LinuxWindow::~LinuxWindow()
	{
		Shutdown();
	}

	void LinuxWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		OE_CORE_INFO("Creating window '{0}' ({1}x{2})", props.Title, props.Width, props.Height);

		if (s_WindowCount == 0)
		{
			int success = glfwInit();
			OE_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			#ifdef OE_DEBUG
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			#endif
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		s_WindowCount++;

		m_Context = GraphicsContext::Create(this);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(false);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event((KeyCode)key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event((KeyCode)key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event((KeyCode)key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event((KeyCode)keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (focused)
			{
				WindowFocusEvent event;
				data.EventCallback(event);
			}
			else
			{
				WindowLostFocusEvent event;
				data.EventCallback(event);
			}
		});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xPos, int yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowMovedEvent event(xPos, yPos);
			data.EventCallback(event);
		});
	}

	void LinuxWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);

		if (--s_WindowCount == 0)
			glfwTerminate();
	}

	void LinuxWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void LinuxWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool LinuxWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void LinuxWindow::SetTitle(const char* title)
	{
		glfwSetWindowTitle(m_Window, title);
	}

	void LinuxWindow::SetMousePosition(Vector2 position)
	{
		glfwSetCursorPos(m_Window, (double)position.x, (double)position.y);
	}

	Vector2 LinuxWindow::GetMousePosition()
	{
		double x, y;
		glfwGetCursorPos(m_Window, &x, &y);
		return { x, y };
	}

	void LinuxWindow::SetClipboardText(const char* text)
	{
		glfwSetClipboardString(m_Window, text);
	}

	const char* LinuxWindow::GetClipboardText()
	{
		return glfwGetClipboardString(m_Window);
	}
}
