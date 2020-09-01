#pragma once

#include "OverEngine/Core/Core.h"

#include <filesystem>

namespace OverEngine
{
	class FileSystem
	{
	public:
		static String ReadFile(const String& path);

		static bool FileExists(const String& path);
		static bool FileExists(const std::filesystem::path& path);

		static bool IsFile(const String& path);
		static bool IsFile(const std::filesystem::path& path);

		static bool IsDirectory(const String& path);
		static bool IsDirectory(const std::filesystem::path& path);

		static String FixFileSystemPath(String path);
		static void FixFileSystemPath(String* path);
		static void FixFileSystemPath(char* path);

		static String ExtractFileNameFromPath(const String& path);
		static void ExtractFileNameFromPath(String* path);

		static String ExtractFileExtentionFromName(const String& name);
	};
}
