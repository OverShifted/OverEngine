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

	String FileSystem::FixPath(String path)
	{
		size_t backSlash = path.find_first_of('\\');
		while (backSlash != String::npos)
		{
			path[backSlash] = '/';
			backSlash = path.find_first_of('\\');
		}

		return path;
	}

	void FileSystem::FixPath(String* path)
	{
		size_t backSlash = path->find_first_of('\\');
		while (backSlash != String::npos)
		{
			(*path)[backSlash] = '/';
			backSlash = path->find_first_of('\\');
		}
	}

	void FileSystem::FixPath(char* path)
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

	////////////////////////////////////////////////////////////////////////
	// FileWatcher /////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	FileWatcher::FileWatcher(String pathToWatch, std::chrono::duration<int, std::milli> delay)
		: m_PathToWatch(m_PathToWatch), m_Delay(m_Delay), m_Running(false), m_Stopped(false)
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(m_PathToWatch))
		{
			m_Paths[entry.path().string()] = std::filesystem::last_write_time(entry);
		}

		m_Action = [](const String&, FileWatcherEvent, void*) {};
	}

	void FileWatcher::Reset(String pathToWatch, std::chrono::duration<int, std::milli> delay)
	{
		m_PathToWatch = pathToWatch;
		m_Delay = delay;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(m_PathToWatch))
		{
			m_Paths[entry.path().string()] = std::filesystem::last_write_time(entry);
		}

		m_Action = [](const String&, FileWatcherEvent, void*) {};
	}

	void FileWatcher::Start(ActionFn action, void* userData)
	{
		m_Running = true;
		m_Action = action;
		m_UserData = userData;
		m_Thread = std::thread([this]() { Thread(); });
	}

	void FileWatcher::Thread()
	{
		while (true)
		{
			if (m_Stopped)
				return;

			std::this_thread::sleep_for(m_Delay);

			if (m_Running)
			{
				auto it = m_Paths.begin();
				while (it != m_Paths.end())
				{
					if (!std::filesystem::exists(it->first))
					{
						m_Action(it->first, FileWatcherEvent::Deleted, m_UserData);
						it = m_Paths.erase(it);
					}
					else
					{
						it++;
					}
				}

				for (auto& entry : std::filesystem::recursive_directory_iterator(m_PathToWatch))
				{
					auto currentFileLastWriteTime = std::filesystem::last_write_time(entry);
					auto pathString = entry.path().string();

					if (m_Paths.find(pathString) == m_Paths.end())
					{
						m_Paths[pathString] = currentFileLastWriteTime;
						m_Action(pathString, FileWatcherEvent::Created, m_UserData);
					}
					else
					{
						if (m_Paths[pathString] != currentFileLastWriteTime)
						{
							m_Paths[pathString] = currentFileLastWriteTime;
							m_Action(pathString, FileWatcherEvent::Modified, m_UserData);

						}
					}
				}
			}
		}
	}
}
