#pragma once

#include <OverEngine.h>

#include <cppfs/Tree.h>
#include <cppfs/FileHandle.h>
#include <future>

namespace OverEditor
{
	using namespace OverEngine;

	class EditorProject;
	class EditorLayer;

	Ref<EditorProject> NewProject(const String& name, const String& directoryPath);

	class EditorProject
	{
	public:
		EditorProject() = default;
		EditorProject(const String& path);

		~EditorProject();

		inline const String& GetName() { return m_Name; }

		inline const String& GetRootPath()            { return m_RootPath; }
		inline const String& GetAssetsDirectoryPath() { return m_AssetsDirectoryPath; }

		inline AssetCollection& GetAssets()           { return m_Assets; }

		String ResolvePhysicalAssetPath(const String& virtualPath);

		// Update file watcher and load assets
		void OnUpdate(TimeStep ts);
	private:
		String m_Name;

		// Paths
		String m_ProjectFilePath;
		String m_RootPath;
		String m_AssetsDirectoryPath;

		// Asset Management
		AssetCollection m_Assets;
		std::unique_ptr<cppfs::Tree> m_LastAssetsFileTree;
		std::unique_ptr<cppfs::Diff> m_LastAssetsDiff;

		std::future<void> m_AssetsDiffFuture;
		cppfs::FileHandle m_AssetsDirectory;
	};
}
