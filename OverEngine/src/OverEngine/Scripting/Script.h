#pragma once

namespace OverEngine::Scripting
{
	class ScriptEngine
	{
	public:
		enum class Language { Lua };

	public:
		~ScriptEngine() = default;

		virtual void Init() = 0;

		// Runs the script and returns the returned value by script
		virtual int Run() = 0;

	protected:
		Language m_Language;
	};
}
