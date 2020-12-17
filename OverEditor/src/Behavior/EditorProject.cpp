#include "EditorProject.h"

#include <OverEngine/Core/Extentions.h>

namespace OverEditor
{
	Ref<EditorProject> NewProject(const String& name, const String& directoryPath)
	{
		if (name.size() == 0 || directoryPath.size() == 0)
			return nullptr;

		String _directoryPath = directoryPath;

		YAML::Node projectNode;
		projectNode["Name"] = name;
		projectNode["AssetsRoot"] = "Assets";

		FileSystem::FixPath(&_directoryPath);

		std::ofstream projectFile;
		String projectRoot = "";
		if (_directoryPath[_directoryPath.size() - 1] == '/')
		{
			std::filesystem::create_directory(_directoryPath + name);
			projectFile.open(_directoryPath + name + "/project." + OE_PROJECT_FILE_EXTENSION, std::ios::out);
			projectRoot = _directoryPath + name;
		}
		else
		{
			std::filesystem::create_directory(_directoryPath + '/' + name);
			projectFile.open(_directoryPath + '/' + name + "/project." + OE_PROJECT_FILE_EXTENSION, std::ios::out);
			projectRoot = _directoryPath + '/' + name;
		}

		projectFile << projectNode;
		std::filesystem::create_directory(projectRoot + "/Assets");
		return CreateRef<EditorProject>(projectRoot + "/project." + OE_PROJECT_FILE_EXTENSION);
	}

	////////////////////////////////////////////////////////////
	// EditorProject ///////////////////////////////////////////
	////////////////////////////////////////////////////////////

	EditorProject::EditorProject(const String& path)
		: m_ProjectFilePath(path)
	{
		auto lastSlash = path.find_last_of("/\\");
		lastSlash = lastSlash == String::npos ? path.size() : lastSlash;
		m_RootPath = path.substr(0, lastSlash);

		YAML::Node projectNode = YAML::LoadFile(path);

		m_Name = projectNode["Name"].as<String>();
		m_AssetsDirectoryPath = m_RootPath + "/" + projectNode["AssetsRoot"].as<String>();

		m_Assets.InitFromAssetsDirectory(m_AssetsDirectoryPath, projectNode["AssetsRootGuid"].as<uint64_t>());

		m_Watcher.Reset(m_AssetsDirectoryPath, std::chrono::milliseconds(250));
		m_Watcher.Start([](const String& path, FileWatcherEvent event, void* userData)
		{
			EditorProject& proj = *static_cast<EditorProject*>(userData);

			if (event == FileWatcherEvent::Created)
			{
				std::lock_guard<std::mutex> lock(proj.GetAssetLoadCommandBufferMutex());
				proj.GetAssetLoadCommandBuffer().push_back(path);
			}

		}, this);
	}

	String EditorProject::ResolvePhysicalAssetPath(const String& virtualPath)
	{
		return m_AssetsDirectoryPath + "/" + virtualPath.substr(9, virtualPath.size());
	}
}
