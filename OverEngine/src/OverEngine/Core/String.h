#pragma once

namespace OverEngine
{
	Ref<Vector<String>> SplitString(const String& string, const String& delimiters);
	Ref<Vector<String>> SplitString(const String& string, const char delimiter);
}
