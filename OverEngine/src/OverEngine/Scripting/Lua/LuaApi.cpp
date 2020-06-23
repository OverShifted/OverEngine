#include "pcheader.h"

#include "LuaApi.h"

#include <lua.hpp>

namespace OverEngine
{

	int LuaApi::PrintInLua(lua_State* L)
	{
		int n = lua_gettop(L);  /* number of arguments */

		std::stringstream ss;

		for (int i = 1; i <= n; i++)
		{
			lua_pushvalue(L, -1);  /* function to be called */
			lua_pushvalue(L, i);   /* value to print */

			ss << lua_tostring(L, -1);
			if (i < n)
				ss << " ";

		}

		OE_CORE_INFO("[LUA] {}", ss.str());
		return 0;
	}

	int LuaApi::WarningInLua(lua_State* L)
	{
		int n = lua_gettop(L);  /* number of arguments */

		std::stringstream ss;

		for (int i = 1; i <= n; i++)
		{
			lua_pushvalue(L, -1);  /* function to be called */
			lua_pushvalue(L, i);   /* value to print */

			ss << lua_tostring(L, -1);
			if (i < n)
				ss << " ";

		}

		OE_CORE_WARN("[LUA] {}", ss.str());
		return 0;
	}

	int LuaApi::ErrorInLua(lua_State* L)
	{
		int n = lua_gettop(L);  /* number of arguments */

		std::stringstream ss;

		for (int i = 1; i <= n; i++)
		{
			lua_pushvalue(L, -1);  /* function to be called */
			lua_pushvalue(L, i);   /* value to print */

			ss << lua_tostring(L, -1);
			if (i < n)
				ss << " ";

		}

		OE_CORE_ERROR("[LUA] {}", ss.str());
		return 0;
	}

}