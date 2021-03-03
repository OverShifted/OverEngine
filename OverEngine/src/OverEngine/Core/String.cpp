#include "pcheader.h"
#include "String.h"

namespace OverEngine
{
	void StringUtils::Split(const String& string, const String& delimiters, Vector<String>& result)
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

	void StringUtils::Join(const Vector<String>& strings, const String& delimiter, String& out)
	{
		size_t i = 0;

		for (const String& item : strings)
		{
			i++;
			
			out += item;
			if (i != strings.size())
				out += delimiter;
		}
	}
}
