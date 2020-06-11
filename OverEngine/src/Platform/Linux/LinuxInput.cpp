#include "pcheader.h"
#include "LinuxInput.h"

#include "OverEngine/Core/Application.h"
#include <GLFW/glfw3.h>

namespace OverEngine
{
	Input* Input::s_Instance = new LinuxInput();

	bool LinuxInput::IsKeyPressedImpl(KeyCode keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetMainWindow().GetNativeWindow());
		auto state = glfwGetKey(window, (int)keycode);
		return state == (int)KeyTrigger::Press;
	}

	bool LinuxInput::IsMouseButtonPressedImpl(KeyCode button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetMainWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, (int)button);
		return state == (int)KeyTrigger::Press;
	}

	std::pair<float, float> LinuxInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetMainWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float LinuxInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePositionImpl(); // language feature 'structured bindings'
		return x;
	}

	float LinuxInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePositionImpl();
		return y;
	}
}