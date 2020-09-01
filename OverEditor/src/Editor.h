#pragma once

#include <OverEngine.h>
using namespace OverEngine;

#include <OverEngine/Core/GUIDGenerator.h>
#include <OverEngine/Assets/AssetResource.h>
#include <OverEngine/Assets/Asset.h>

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

	inline void EditScene(const Ref<Scene>& scene, String path) {
		m_OpenScene = scene;
		m_OpenScenePath = path;
		m_SelectedEntities.clear();
	}
	inline Ref<Scene> GetScene() { return m_OpenScene; }
private:
	void OnMainMenubarGUI();
	void OnMainDockSpaceGUI();
	void OnToolbarGUI();
	
	void OnProjectManagerGUI();

	void OnViewportGUI();
	void OnSceneGraphGUI();
	void OnInspectorGUI();

	void OnAssetsGUI();
	void OnAssetImportGUI();

	void OnConsoleGUI();
private:
	Ref<EditorProject> m_EditingProject;

	bool m_IsProjectManagerOpen = true;
	bool m_IsSceneGraphOpen     = true;
	bool m_IsViewportOpen       = true;
	bool m_IsAssetsBrowserOpen  = true;

	Ref<Scene> m_OpenScene;
	String m_OpenScenePath;

	Ref<FrameBuffer> m_ViewportFrameBuffer;
	Vector2 m_ViewportSize;
	Vector<Entity> m_SelectedEntities;

	Vector<String> m_SelectedAssetFiles;
	//Vector<Guid> m_SelectedResources;
	bool m_IsAssetRootSelected;
	bool m_AssetBrowserOneColumnView = true;
	int m_AssetThumbnailSize = 100;
	int m_AssetThumbnailSizeMin = 50;
	int m_AssetThumbnailSizeMax = 300;
};
