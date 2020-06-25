#include "pcheader.h"
#include "FileSystem.h"

#include <fstream>

namespace OverEngine
{
	namespace FileSystem
	{

		String ReadFile(const String& path)
		{
			String result;
			std::ifstream in(path, std::ios::in | std::ios::binary);
			if (in)
			{
				in.seekg(0, std::ios::end);
				result.resize(in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&result[0], result.size());
				in.close();
			}
			else
			{
				OE_CORE_ERROR("Could not open file '{0}'", path);
			}

			return result;
		}

	}
}