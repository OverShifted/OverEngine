#include "pcheader.h"
#include "String.h"

namespace OverEngine
{
	void SplitString(const String& string, const String& delimiters, Vector<String>& result)
	{
		size_t start = 0;
		size_t end = string.find_first_of(delimiters);

		while (end <= String::npos)
		{
			String token = string.substr(start, end - start);
			if (!token.empty())
				result.push_back(token);

			if (end == String::npos)
				break;

			start = end + 1;
			end = string.find_first_of(delimiters, start);
		}
	}

	void SplitString(const String& string, const char delimiter, Vector<String>& result)
	{
		return SplitString(string, String(1, delimiter), result);
	}
}
