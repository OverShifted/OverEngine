#pragma once

#include "OverEngine/Core/Core.h"

#include "imgui.h"

namespace OverEngine
{
	struct OVER_API Vector2 {
		float x;
		float y;

		Vector2(float _x = 0, float _y = 0)
			: x(_x), y(_y)
		{
		}

		void Set(float _x, float _y);

		std::string ToString() const;
		inline operator ImVec2() const { return ImVec2(x, y); }
	};

	inline std::ostream& operator<<(std::ostream& os, const Vector2& v)
	{
		return os << v.ToString();
	}
	
}