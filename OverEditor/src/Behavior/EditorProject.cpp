#include "EditorProject.h"

#include <OverEngine/Core/Extentions.h>
#include <OverEngine/Core/FileSystem/FileSystem.h>

#include <cppfs/fs.h>
#include <cppfs/Diff.h>
#include <cppfs/Change.h>

#include <future>

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

	static void CalculateDiff(
		std::unique_ptr<cppfs::Diff>* diff_ptr,
		cppfs::FileHandle* assetsDir_ptr,
		std::unique_ptr<cppfs::Tree>* lastTree_ptr)
	{
		auto& diff = *diff_ptr;
		auto& assetsDir = *assetsDir_ptr;
		auto& lastTree = *lastTree_ptr;

		// Calculate diff of assets directory since last update
		auto newAssetFileTree = assetsDir.readTree();
		diff = std::move(lastTree->createDiff(*newAssetFileTree));
		lastTree = std::move(newAssetFileTree);

		for (const auto& change : diff->changes())
		{
			OE_INFO(change.toString());
		}
	}

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
		m_AssetsDirectory = cppfs::fs::open(m_AssetsDirectoryPath);
		m_AssetsDiffFuture = std::async(std::launch::async,
			CalculateDiff, &m_LastAssetsDiff, &m_AssetsDirectory, &m_LastAssetsFileTree
		);
		m_LastAssetsFileTree = m_AssetsDirectory.readTree();
	}


	EditorProject::~EditorProject()
	{
	}

	String EditorProject::ResolvePhysicalAssetPath(const String& virtualPath)
	{
		return m_AssetsDirectoryPath + "/" + virtualPath.substr(9, virtualPath.size());
	}

	void EditorProject::OnUpdate(TimeStep ts)
	{
		if (STD_FUTURE_IS_DONE(m_AssetsDiffFuture))
		{
			//OE_INFO("LAUNCH");
			m_AssetsDiffFuture = std::async(std::launch::async,
				CalculateDiff, &m_LastAssetsDiff, &m_AssetsDirectory, &m_LastAssetsFileTree
			);
		}
		else
		{
			//OE_INFO("-");
		}
	}
}
