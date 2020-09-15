#include "pcheader.h"

#include "LuaScript.h"
#include "LuaApi.h"

#include <lua.hpp>

namespace OverEngine
{
	namespace Scripting
	{
		LuaScript::LuaScript(String filePathOrCodeString, CodeLoadingMode codeLoadMode)
			: Script(Kind::Lua, codeLoadMode), m_State(luaL_newstate()), m_CodeOrPath(filePathOrCodeString)
		{
			luaopen_base(m_State);

			lua_register(m_State, "print", LuaApi::PrintInLua);
			lua_register(m_State, "warn", LuaApi::WarningInLua);
			lua_register(m_State, "error", LuaApi::ErrorInLua);
		}

		LuaScript::~LuaScript()
		{
			lua_close(m_State);
		}

		int LuaScript::Run()
		{
			if (m_CodeLoadMode == Script::CodeLoadingMode::File)
				return luaL_dofile(m_State, m_CodeOrPath.c_str());
			else
				return luaL_dostring(m_State, m_CodeOrPath.c_str());
		}
	}
}
