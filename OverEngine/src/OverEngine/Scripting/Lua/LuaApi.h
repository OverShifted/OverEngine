#pragma once

struct lua_State;

namespace OverEngine
{
	class LuaApi
	{
	public:
		static int PrintInLua(lua_State* L);
		static int WarningInLua(lua_State* L);
		static int ErrorInLua(lua_State* L);
	};
}