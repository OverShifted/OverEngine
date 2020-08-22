#pragma once

namespace OverEngine
{
	Vector<String> SplitString(const String& string, const String& delimiters);
	Vector<String> SplitString(const String& string, const char delimiter);
}