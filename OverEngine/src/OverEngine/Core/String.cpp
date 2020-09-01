#include "pcheader.h"
#include "String.h"

namespace OverEngine
{
	Ref<Vector<String>> SplitString(const String& string, const String& delimiters)
	{
		size_t start = 0;
		size_t end = string.find_first_of(delimiters);

		Ref<Vector<String>> result = CreateRef<Vector<String>>();

		while (end <= String::npos)
		{
			String token = string.substr(start, end - start);
			if (!token.empty())
				result->push_back(token);

			if (end == String::npos)
				break;

			start = end + 1;
			end = string.find_first_of(delimiters, start);
		}

		return result;
	}

	Ref<Vector<String>> SplitString(const String& string, const char delimiter)
	{
		return SplitString(string, String(1, delimiter));
	}
}
