#pragma once

#include <sol/sol.hpp>

namespace OverEngine
{
	class LuaScriptingEngine
	{
	public:
		LuaScriptingEngine() = default;

		const auto& GetSolState() const { return m_State; }
		auto& GetSolState() { return m_State; }
	private:
		sol::state m_State;
	};
}
