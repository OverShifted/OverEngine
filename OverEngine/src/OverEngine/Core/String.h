#pragma once

namespace OverEngine
{
	void SplitString(const String& string, const String& delimiters, Vector<String>& result);
	void SplitString(const String& string, const char delimiter, Vector<String>& result);
}
