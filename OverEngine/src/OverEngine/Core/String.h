#pragma once

#include <OverEngine/Core/Core.h>

namespace OverEngine
{
	class StringUtils
	{
	public:
		static void Split(const String& string, const String& delimiters, Vector<String>& result);

		static void Join(const Vector<String>& strings, const String& delimiter, String& out);
	};
}
