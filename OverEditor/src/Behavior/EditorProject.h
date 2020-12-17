#pragma once

#include <OverEngine.h>

#include <OverEngine/Core/FileSystem/FileSystem.h>

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

		inline const String& GetName() { return m_Name; }

		inline const String& GetRootPath() { return m_RootPath; }
		inline const String& GetAssetsDirectoryPath() { return m_AssetsDirectoryPath; }
		inline AssetCollection& GetAssets() { return m_Assets; }
		inline auto& GetAssetLoadCommandBuffer() { return m_AssetLoadCommandBuffer; }
		inline auto& GetAssetLoadCommandBufferMutex() { return m_AssetLoadCommandBufferMutex; }

		String ResolvePhysicalAssetPath(const String& virtualPath);
	private:
		String m_Name;

		String m_ProjectFilePath;
		String m_RootPath;
		String m_AssetsDirectoryPath;

		AssetCollection m_Assets;

		FileWatcher m_Watcher;

		Vector<String> m_AssetLoadCommandBuffer;
		std::mutex m_AssetLoadCommandBufferMutex;
	};
}
