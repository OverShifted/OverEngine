#pragma once

#include "OverEngine/Core/Core.h"

#include <filesystem>

namespace OverEngine
{
	class FileSystem
	{
	public:
		static String ReadFile(const String& path);

		static String FixPath(String path);
		static void FixPath(String* pathPtr);
		static void FixPath(char* path);
	};
}
