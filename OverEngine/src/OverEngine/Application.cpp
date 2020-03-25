#include "pcheader.h"
#include "Application.h"

#include "OverEngine/Events/ApplicationEvent.h"
#include "OverEngine/Log.h"

#include <GLFW/glfw3.h>

namespace OverEngine
{
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
			GLFWwindow* a = m_Window.get()->GetWindow();
			
			//glfwSetInputMode(a, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			double x = 0.0, y = 0.0;
			glfwGetCursorPos(a, &x, &y);
			//double xc = *x;
			//double yc = *y;
			if (0 < y && y < 900) {
				if (x <= 0.0)
					glfwSetCursorPos(a, 1600, y);
				else if (x >= 1600)
					glfwSetCursorPos(a, 0, y);
			}
		}
	}
}