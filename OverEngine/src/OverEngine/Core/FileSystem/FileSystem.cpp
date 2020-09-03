#include "pcheader.h"
#include "FileSystem.h"

#include <fstream>
#include <filesystem>

namespace OverEngine
{
	String FileSystem::ReadFile(const String& path)
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

	bool FileSystem::FileExists(const String& path)
	{
		return std::filesystem::exists(path);
	}

	bool FileSystem::FileExists(const std::filesystem::path& path)
	{
		return std::filesystem::exists(path);
	}

	bool FileSystem::IsFile(const String& path)
	{
		return !IsDirectory(path);
	}

	bool FileSystem::IsFile(const std::filesystem::path& path)
	{
		return !IsDirectory(path);
	}

	bool FileSystem::IsDirectory(const String& path)
	{
		return std::filesystem::is_directory(path);
	}

	bool FileSystem::IsDirectory(const std::filesystem::path& path)
	{
		return std::filesystem::is_directory(path);
	}

	String FileSystem::FixFileSystemPath(String path)
	{
		size_t backSlash = path.find_first_of('\\');
		while (backSlash != String::npos)
		{
			path[backSlash] = '/';
			backSlash = path.find_first_of('\\');
		}

		return path;
	}

	void FileSystem::FixFileSystemPath(String* path)
	{
		size_t backSlash = path->find_first_of('\\');
		while (backSlash != String::npos)
		{
			(*path)[backSlash] = '/';
			backSlash = path->find_first_of('\\');
		}
	}

	void FileSystem::FixFileSystemPath(char* path)
	{
		for (uint32_t i = 0; path[i] != '\0'; i++)
			if (path[i] == '\\')
				path[i] = '/';
	}

	String FileSystem::ExtractFileNameFromPath(const String& path)
	{
		auto lastSlash = path.find_last_of("/\\");
		lastSlash = lastSlash == String::npos ? 0 : lastSlash + 1;
		return path.substr(lastSlash, path.size() - lastSlash);
	}

	void FileSystem::ExtractFileNameFromPath(String* path)
	{
		auto lastSlash = path->find_last_of("/\\");
		lastSlash = lastSlash == String::npos ? 0 : lastSlash + 1;
		(*path) = path->substr(lastSlash, path->size() - lastSlash);
	}

	String FileSystem::ExtractFileExtentionFromName(const String& name)
	{
		auto lastDot = name.rfind('.');
		lastDot = lastDot == String::npos ? 0 : lastDot + 1;
		return name.substr(lastDot, name.size() - lastDot);
	}

	String FileSystem::ExtractFileExtentionFromPath(const String& path)
	{
		return ExtractFileExtentionFromName(ExtractFileNameFromPath(path));
	}
}
