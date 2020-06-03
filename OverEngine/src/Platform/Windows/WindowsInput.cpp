#include "pcheader.h"
#include "WindowsInput.h"

#include "OverEngine/Core/Application.h"
#include <GLFW/glfw3.h>

namespace OverEngine
{
	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(KeyCode keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetMainWindow().GetNativeWindow());
		auto state = glfwGetKey(window, (int)keycode);
		return state == (int)KeyTrigger::Press;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(KeyCode button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetMainWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, (int)button);
		return state == (int)KeyTrigger::Press;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetMainWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePositionImpl(); // language feature 'structured bindings'
		return x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePositionImpl();
		return y;
	}
}