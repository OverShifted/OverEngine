#include "pcheader.h"
#include "OverEngine/Input/Input.h"

#include "OverEngine/Core/Runtime/Application.h"
#include <GLFW/glfw3.h>

#define GET_NATIVE_WINDOW() static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow())

namespace OverEngine
{
	bool Input::IsKeyPressed(KeyCode keycode)
	{
		auto window = GET_NATIVE_WINDOW();
		auto state = glfwGetKey(window, (int)keycode);
		return state == (int)KeyTrigger::Press;
	}

	bool Input::IsMouseButtonPressed(KeyCode button)
	{
		auto window = GET_NATIVE_WINDOW();
		auto state = glfwGetMouseButton(window, (int)button);
		return state == (int)KeyTrigger::Press;
	}

	Vector2 Input::GetMousePosition()
	{
		auto window = GET_NATIVE_WINDOW();
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}
}
