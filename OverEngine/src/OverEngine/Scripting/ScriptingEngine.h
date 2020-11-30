#pragma once

#include <sol/sol.hpp>

namespace OverEngine
{
	class ScriptingEngine
	{
	public:
		static void LoadApi(sol::state& state);
	};
}
