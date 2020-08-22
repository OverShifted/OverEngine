#pragma once

#include "OverEngine/Core/Core.h"

namespace OverEngine
{
	class FileSystem
	{
	public:
		static String ReadFile(const String& path);
		static bool FileExists(const String& path);

		static String FixFileSystemPath(String path);
		static void FixFileSystemPath(String* path);
		static void FixFileSystemPath(char* path);

		static String ExtractFileNameFromPath(const String& path);
		static void ExtractFileNameFromPath(String* path);
	};
}