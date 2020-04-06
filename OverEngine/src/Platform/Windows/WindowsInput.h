#pragma once

#include "OverEngine/Input/Input.h"

namespace OverEngine
{
	class OVER_API WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(KeyCode keycode) override;

		virtual bool IsMouseButtonPressedImpl(KeyCode button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	};
}