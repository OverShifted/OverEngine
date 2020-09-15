#pragma once

namespace OverEngine
{
	namespace Scripting
	{
		class Script
		{
		public:
			enum class Kind { Lua };
			enum class CodeLoadingMode { CodeString, File };
		protected:
			Script(Kind kind, CodeLoadingMode codeLoadMode)
				: m_Kind(kind), m_CodeLoadMode(codeLoadMode)
			{
			}

		public:
			~Script() = default;
			virtual int Run() = 0;

		protected:
			Kind m_Kind;
			CodeLoadingMode m_CodeLoadMode;
		};
	}
}
