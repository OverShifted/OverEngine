#pragma once

#include <OverEngine.h>

#include <OverEngine/Core/FileSystem/FileSystem.h>

namespace OverEditor
{
	using namespace OverEngine;

	class EditorProject;
	class EditorLayer;

	Ref<EditorProject> NewProject(const String& name, String directoryPath);

	class EditorProject
	{
	public:
		EditorProject() = default;
		EditorProject(const String& path);

		inline const String& GetName() { return m_Name; }

		inline const String& GetRootPath() { return m_RootPath; }
		inline const String& GetAssetsDirectoryPath() { return m_AssetsDirectoryPath; }
		inline AssetCollection& GetAssets() { return m_Assets; }

		String ResolvePhysicalAssetPath(const String& virtualPath);
	private:
		String m_Name;

		String m_ProjectFilePath;
		String m_RootPath;
		String m_AssetsDirectoryPath;

		AssetCollection m_Assets;

		FileWatcher m_Watcher;
	};
}
