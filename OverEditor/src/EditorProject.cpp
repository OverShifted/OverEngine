#include "EditorProject.h"

#include <OverEngine/Core/Extentions.h>

namespace OverEditor
{
	Ref<EditorProject> NewProject(const String& name, String directoryPath)
	{
		if (name.size() == 0 || directoryPath.size() == 0)
			return nullptr;

		YAML::Node projectNode;
		projectNode["Name"] = name;
		projectNode["AssetsRoot"] = "Assets";

		FileSystem::FixPath(&directoryPath);

		std::ofstream projectFile;
		String projectRoot = "";
		if (directoryPath[directoryPath.size() - 1] == '/')
		{
			std::filesystem::create_directory(directoryPath + name);
			projectFile.open(directoryPath + name + "/project." + OE_PROJECT_FILE_EXTENSION, std::ios::out);
			projectRoot = directoryPath + name;
		}
		else
		{
			std::filesystem::create_directory(directoryPath + '/' + name);
			projectFile.open(directoryPath + '/' + name + "/project." + OE_PROJECT_FILE_EXTENSION, std::ios::out);
			projectRoot = directoryPath + '/' + name;
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
		m_Watcher.Start([](String s, FileWatcherEvent e)
		{
			String message;
			if (e == FileWatcherEvent::Created)
				message = "Created";
			else if (e == FileWatcherEvent::Deleted)
				message = "Deleted";
			else
				message = "Modified";
			OE_CORE_INFO("file : {}, {}", s, message);
		});
	}

	String EditorProject::ResolvePhysicalAssetPath(const String& virtualPath)
	{
		return m_AssetsDirectoryPath + "/" + virtualPath.substr(9, virtualPath.size());
	}

}
