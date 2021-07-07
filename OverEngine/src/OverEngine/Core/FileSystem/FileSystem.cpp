#include "pcheader.h"
#include "FileSystem.h"

#include <fstream>
#include <filesystem>

namespace OverEngine
{
	String FileSystem::ReadFile(const String& path)
	{
		String result;
		std::ifstream in(path, std::ios::binary);

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
			OE_CORE_ASSERT("Could not open file '{0}'", path);
		}

		return result;
	}

	String FileSystem::FixPath(String path)
	{
		size_t backSlash = path.find_first_of('\\');
		size_t nextCharPos = backSlash + 1;
		char nextChar = '\0';

		while (backSlash != String::npos)
		{
			path[backSlash] = '/';

			if (nextCharPos < path.size())
			{
				nextChar = path[nextCharPos];

				if (nextChar == '/' || nextChar == '\\')
					path.erase(nextCharPos);
			}

			backSlash = path.find_first_of('\\');
			nextCharPos = backSlash + 1;
		}

		return path;
	}

	void FileSystem::FixPath(String* path_ptr)
	{
		String& path = *path_ptr;

		size_t backSlash = path.find_first_of('\\');
		size_t nextCharPos = backSlash + 1;
		char nextChar = '\0';

		while (backSlash != String::npos)
		{
			path[backSlash] = '/';

			if (nextCharPos < path.size())
			{
				nextChar = path[nextCharPos];
				
				if (nextChar == '/' || nextChar == '\\')
					path.erase(nextCharPos);
			}

			backSlash = path.find_first_of('\\');
			nextCharPos = backSlash + 1;
		}
	}

	void FileSystem::FixPath(char* path)
	{
		for (uint32_t i = 0; path[i] != '\0'; i++)
			if (path[i] == '\\')
				path[i] = '/';
	}
}
