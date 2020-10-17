#pragma once

#include "OverEngine/Scripting/Script.h"

struct lua_State;

namespace OverEngine::Scripting
{
	class LuaScript : public ScriptEngine
	{
	public:
		LuaScript(String filePathOrCodeString);
		~LuaScript();

		virtual int Run() override;

		inline lua_State* GetLuaState() const { return m_State; }
	private:
		lua_State* m_State;
		String m_CodeOrPath;
	};
}
