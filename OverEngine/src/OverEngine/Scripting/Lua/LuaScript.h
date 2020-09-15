#pragma once

#include "OverEngine/Scripting/Script.h"

struct lua_State;

namespace OverEngine
{
	namespace Scripting
	{
		class LuaScript : public Script
		{
		public:
			LuaScript(String filePathOrCodeString, CodeLoadingMode codeLoadMode = CodeLoadingMode::File);
			~LuaScript();

			virtual int Run() override;

			inline lua_State* GetLuaState() const { return m_State; }
		private:
			lua_State* m_State;
			String m_CodeOrPath;
		};
	}
}
