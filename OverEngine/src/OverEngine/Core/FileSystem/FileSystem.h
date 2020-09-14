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

		FileWatcher(String pathToWatch, std::chrono::duration<int, std::milli> delay);
			
		FileWatcher()
			: m_PathToWatch(""), m_Delay(std::chrono::milliseconds(5000)), m_Running(false), m_Stopped(false)
		{
			m_Action = [](String, FileWatcherEvent) {};
		}

		~FileWatcher()
		{
			m_Stopped = true;
			m_Thread.join();
		}

		void Reset(String pathToWatch, std::chrono::duration<int, std::milli> delay);
		void Start(void(*action)(String, FileWatcherEvent));
	private:
		void Thread();
	private:
		std::thread m_Thread;
		std::unordered_map<String, std::filesystem::file_time_type> m_Paths;
		void(*m_Action)(String, FileWatcherEvent);
		bool m_Stopped = false;
	};
}
