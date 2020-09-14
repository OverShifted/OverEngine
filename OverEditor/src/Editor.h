#pragma once

#include <OverEngine.h>
using namespace OverEngine;

#include <OverEngine/Core/GUIDGenerator.h>
#include <OverEngine/Assets/Resource.h>
#include <OverEngine/Assets/Asset.h>
#include <OverEngine/Core/FileSystem/FileSystem.h>

#include "ViewportPanel.h"

class EditorProject;

Ref<EditorProject> NewProject(const String& name, String directoryPath);

class EditorProject
{
public:
	EditorProject() = default;
	EditorProject(const String& path);

	inline const String& GetRootPath() { return m_RootPath; }
	inline const String& GetAssetsDirectoryPath() { return m_AssetsDirectoryPath; }
	inline ResourceCollection& GetResources() { return m_Resources; }

	String ResolvePhysicalAssetPath(const String& virtualPath);
private:
	
private:
	String m_Name;

	String m_ProjectFilePath;
	String m_RootPath;
	String m_AssetsDirectoryPath;

	ResourceCollection m_Resources;

	FileWatcher m_Watcher;
};

class Editor
{
public:
	Editor();

	void OnUpdate();
	void OnImGuiRender();
	void OnEvent(Event& event);

	inline void EditProject(const Ref<EditorProject>& project) { m_EditingProject = project; }
	inline Ref<EditorProject> GetProject() { return m_EditingProject; }

	inline void EditScene(const Ref<Scene>& scene, String path)
	{
		m_OpenScene = scene;
		m_OpenScenePath = path;
		m_ViewportPanel.SetContext(m_OpenScene);
		m_SelectedEntities.clear();
	}

	inline Ref<Scene> GetScene() { return m_OpenScene; }
private:
	void OnMainMenubarGUI();
	void OnMainDockSpaceGUI();
	void OnToolbarGUI();
	
	void OnProjectManagerGUI();

	void OnSceneGraphGUI();
	void OnInspectorGUI();

	void OnAssetsGUI();
	void OnAssetImportGUI();

	void OnConsoleGUI();
private:
	Ref<EditorProject> m_EditingProject;
	Ref<Texture2D> m_IconsTexture;
	UnorderedMap<String, Ref<Texture2D>> m_Icons;

	bool m_IsProjectManagerOpen = true;
	bool m_IsSceneGraphOpen     = true;
	bool m_IsViewportOpen       = true;
	bool m_IsAssetsBrowserOpen  = true;

	Ref<Scene> m_OpenScene;
	String m_OpenScenePath;
	Vector<Entity> m_SelectedEntities;

	ViewportPanel m_ViewportPanel;

	Vector<Ref<Resource>> m_SelectedResources;
	bool m_AssetBrowserOneColumnView = false;
	int m_AssetThumbnailSize = 100;
	int m_AssetThumbnailSizeMin = 50;
	int m_AssetThumbnailSizeMax = 300;
};
