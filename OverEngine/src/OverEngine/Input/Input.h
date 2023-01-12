#pragma once

#include "OverEngine/Core/Core.h"

#include "KeyCodes.h"

namespace OverEngine
{
	class Input
	{
	public:
		// TODO: Maybe merge these two functions?
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(KeyCode button);

		static Vector2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
