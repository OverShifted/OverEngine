#pragma once

#include <OverEngine.h>
using namespace OverEngine;

#include <OverEngine/Assets/Asset.h>
#include <OverEngine/Assets/AssetCollection.h>
#include <OverEngine/Core/FileSystem/FileSystem.h>

#include "SceneEditor.h"
#include "Panels/ViewportPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/AssetsPanel.h"

namespace OverEditor
{
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

	class Editor
	{
	public:
		Editor();

		void OnUpdate();
		void OnImGuiRender();

		inline Ref<EditorProject>& GetProject() { return m_EditingProject; }
		inline UnorderedMap<String, Ref<Texture2D>>& GetIcons() { return m_Icons; }

		void EditScene(const Ref<Scene>& scene, String path);
	private:
		void OnProjectManagerGUI();
	private:
		Ref<EditorProject> m_EditingProject;
		Ref<SceneEditor> m_SceneContext;

		// Editor Icons
		Ref<Texture2D> m_IconsTexture;
		UnorderedMap<String, Ref<Texture2D>> m_Icons;

		// Panels
		ViewportPanel m_ViewportPanel;
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ConsolePanel m_ConsolePanel;
		AssetsPanel m_AssetsPanel;

		bool m_IsProjectManagerOpen = true;

		friend class EditorLayer;
	};
}
