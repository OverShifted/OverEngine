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
		static String ExtractFileExtentionFromPath(const String& path);
	};

	enum class FileWatcherEvent { Created, Modified, Deleted };

	class FileWatcher
	{
	public:

		String m_PathToWatch;
		std::chrono::duration<int, std::milli> m_Delay;
		bool m_Running;

		FileWatcher(String pathToWatch, std::chrono::duration<int, std::milli> delay)
			: m_PathToWatch(m_PathToWatch), m_Delay(m_Delay), m_Running(false), m_Stopped(false)
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(m_PathToWatch))
			{
				m_Paths[entry.path().string()] = std::filesystem::last_write_time(entry);
			}

			m_Action = [](String, FileWatcherEvent) -> void {};
		}

		FileWatcher()
			: m_PathToWatch(""), m_Delay(std::chrono::milliseconds(5000)), m_Running(false), m_Stopped(false)
		{
			m_Action = [](String, FileWatcherEvent) -> void {};
		}

		~FileWatcher()
		{
			m_Stopped = true;
			m_Thread.join();
		}

		void Reset(String pathToWatch, std::chrono::duration<int, std::milli> delay)
		{
			m_PathToWatch = pathToWatch;
			m_Delay = delay;
			
			for (const auto& entry : std::filesystem::recursive_directory_iterator(m_PathToWatch))
			{
				m_Paths[entry.path().string()] = std::filesystem::last_write_time(entry);
			}

			m_Action = [](String, FileWatcherEvent) -> void {};
		}

		void Start(const std::function<void(String, FileWatcherEvent)>& action)
		{
			m_Action = action;
			m_Running = true;
			m_Thread = std::thread([this]() { Thread(); });
		}

	private:
		void Thread()
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
							m_Action(it->first, FileWatcherEvent::Deleted);
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

						if (m_Paths.find(entry.path().string()) == m_Paths.end())
						{
							m_Paths[entry.path().string()] = currentFileLastWriteTime;
							m_Action(entry.path().string(), FileWatcherEvent::Created);
						}
						else
						{
							if (m_Paths[entry.path().string()] != currentFileLastWriteTime)
							{
								m_Paths[entry.path().string()] = currentFileLastWriteTime;
								m_Action(entry.path().string(), FileWatcherEvent::Modified);

							}
						}
					}
				}
			}
		}
	private:
		std::thread m_Thread;
		std::unordered_map<String, std::filesystem::file_time_type> m_Paths;
		std::function<void(String, FileWatcherEvent)> m_Action;
		bool m_Stopped = false;
	};
}
