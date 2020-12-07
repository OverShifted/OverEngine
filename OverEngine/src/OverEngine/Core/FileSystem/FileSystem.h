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

		static String FixPath(String path);
		static void FixPath(String* path);
		static void FixPath(char* path);

		static String ExtractFileNameFromPath(const String& path);
		static void ExtractFileNameFromPath(String* path);

		static String ExtractFileExtentionFromName(const String& name);
		static String ExtractFileExtentionFromPath(const String& path);
	};

	enum class FileWatcherEvent { Created, Modified, Deleted };

	class FileWatcher
	{
	public:
		typedef void(*ActionFn)(const String&, FileWatcherEvent, void*);

		String m_PathToWatch;
		std::chrono::duration<int, std::milli> m_Delay;
		bool m_Running;

		FileWatcher(String pathToWatch, std::chrono::duration<int, std::milli> delay);
			
		FileWatcher()
			: m_PathToWatch(""), m_Delay(std::chrono::milliseconds(5000)), m_Running(false), m_Stopped(false)
		{
			m_Action = [](const String&, FileWatcherEvent, void*) {};
		}

		~FileWatcher()
		{
			m_Stopped = true;
			m_Thread.join();
		}

		void Reset(String pathToWatch, std::chrono::duration<int, std::milli> delay);
		void Start(ActionFn action, void* userData);
	private:
		void Thread();
	private:
		bool m_Stopped = false;
		std::thread m_Thread;
		ActionFn m_Action;
		void* m_UserData = nullptr;

		std::unordered_map<String, std::filesystem::file_time_type> m_Paths;
	};
}
