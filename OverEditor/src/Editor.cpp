#include "Editor.h"

#include "SceneLoaderAndSaver.h"
#include "ComponentEditor.h"

#include <OverEngine/ImGui/ExtraImGui.h>
#include <OverEngine/Core/FileSystem/FileSystem.h>
#include <OverEngine/Core/Extentions.h>

#include <imgui/imgui.h>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <json.hpp>

#include <fstream>
#include <filesystem>

Ref<EditorProject> NewProject(const String& name, String directoryPath)
{
	if (name.size() == 0 || directoryPath.size() == 0)
		return nullptr;

	nlohmann::json json;
	json["Name"] = name;
	json["AssetsRoot"] = "Assets";

	FileSystem::FixFileSystemPath(&directoryPath);

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

	projectFile << json.dump(1, '\t');

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

	nlohmann::json projectJson;
	std::ifstream projectFile(path);
	projectFile >> projectJson;
	projectFile.close();

	m_Name = projectJson["Name"];
	m_AssetsDirectoryPath = m_RootPath + "/" + projectJson["AssetsRoot"].get<String>();
	m_Resources.InitFromAssetsDirectory(m_AssetsDirectoryPath, projectJson["AssetsRootGuid"].get<String>());

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

////////////////////////////////////////////////////////////
// Editor //////////////////////////////////////////////////
////////////////////////////////////////////////////////////

Editor::Editor()
{
	FrameBufferProps fbProps;
	fbProps.Width = 720;
	fbProps.Height = 1280;
	m_ViewportFrameBuffer = FrameBuffer::Create(fbProps);
}

/////////////////////////////////////////////
// Main /////////////////////////////////////
/////////////////////////////////////////////

void Editor::OnUpdate()
{
	if (m_OpenScene)
	{
		if (FrameBufferProps props = m_ViewportFrameBuffer->GetProps();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(props.Width != m_ViewportSize.x || props.Height != m_ViewportSize.y))
		{
			m_ViewportFrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
	}

	RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	RenderCommand::Clear();

	m_ViewportFrameBuffer->Bind();

	if (m_OpenScene)
	{
		m_OpenScene->OnTransformUpdate();
		m_OpenScene->OnRender(m_ViewportSize);
	}
	else
	{
		RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		RenderCommand::Clear();
	}

	m_ViewportFrameBuffer->Unbind();
}

void Editor::OnImGuiRender()
{
	OnMainMenubarGUI();
	OnMainDockSpaceGUI();

	OnProjectManagerGUI();
	OnToolbarGUI();

	if (m_EditingProject)
	{
		OnViewportGUI();
		OnSceneGraphGUI();
		OnInspectorGUI();

		OnAssetsGUI();
		OnAssetImportGUI();
	}

	//OnConsoleGUI();
}

void Editor::OnEvent(Event& event)
{
}

void Editor::OnMainMenubarGUI()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Project", "Ctrl+Shift+N")) { m_IsProjectManagerOpen = true; }
			if (ImGui::MenuItem("Open Project", "Ctrl+Shift+O")) { m_IsProjectManagerOpen = true; }
			if (ImGui::MenuItem("Quit Editor", "Alt+F4")) { Application::Get().Close(); }

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void Editor::OnMainDockSpaceGUI()
{
	static bool* p_open = new bool(true);
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 255));
	ImGui::Begin("DockSpace", p_open, window_flags);
	ImGui::PopStyleVar();
	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();
	ImGui::PopStyleColor();
}

void Editor::OnProjectManagerGUI()
{
	static char projectName[32] = "";
	static char projectDirectory[1024] = "";

	static char oepPath[1024] = "";

	if (m_IsProjectManagerOpen)
	{
		if (ImGui::Begin("Project Manager", &m_IsProjectManagerOpen))
		{
			if (ImGui::CollapsingHeader("New Project"))
			{
				ImGui::InputText("Project Name", projectName, 32);
				ImGui::InputText("Project Directory", projectDirectory, 1024);

				if (ImGui::Button("Create and start editing"))
				{
					if (auto project = NewProject(projectName, projectDirectory))
					{
						EditProject(project);
					}
				}
			}

			ImGui::Separator();

			if (ImGui::Button("Open Project"))
			{
				std::stringstream extension;
				extension << "*." << OE_PROJECT_FILE_EXTENSION;
				const char* filters[] = { extension.str().c_str() };
				if (char* filePath = const_cast<char*>(tinyfd_openFileDialog("Open Project", "", 1, filters, "OverEngine Project", 0)))
				{
					FileSystem::FixFileSystemPath(filePath);

					auto project = CreateRef<EditorProject>(filePath);
					EditProject(project);
				}
			}

			ImGui::Separator();

			if (m_EditingProject)
			{
				if (ImGui::Button("Create Scene"))
				{
					std::stringstream extension;
					extension << "*." << OE_SCENE_FILE_EXTENSION;
					const char* filters[] = { extension.str().c_str() };
					if (auto scenePath = tinyfd_saveFileDialog("Create Scene", "", 1, filters, "OverEngine Scene"))
					{
						EditScene(CreateSceneOnDisk(scenePath), scenePath);
					}
				}
			}
		}

		ImGui::End();
	}
}

void Editor::OnToolbarGUI()
{

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 8.0f, 4.0f });
	ImGui::Begin("Toolbar");

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 8.0f, 5.0f });

	if (ImGui::Button("New Project"))
	{
		m_IsProjectManagerOpen = true;
	}

	ImGui::SameLine();

	if (ImGui::Button("Open Project"))
	{
		m_IsProjectManagerOpen = true;
	}

	if (m_OpenScene)
	{
		ImGui::SameLine();

		if (ImGui::Button("Save Scene"))
		{
			SaveSceneToFile(m_OpenScenePath, m_OpenScene);
		}
	}

	ImGui::PopStyleVar();

	ImGui::End();
	ImGui::PopStyleVar();
}

/////////////////////////////////////////////
// Scene ////////////////////////////////////
/////////////////////////////////////////////

void Editor::OnViewportGUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

	ImGui::Begin("Viewport");

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)(intptr_t)m_ViewportFrameBuffer->GetColorAttachmentRendererID(), viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	ImGui::End();

	ImGui::PopStyleVar();
}

static Entity SceneGraphTreeViewRecursive(Entity parentEntity, int* selection_mask, bool fistCall = false, Ref<Scene> scene = nullptr)
{
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

	Entity selectedEntity;

	Vector<Entity>* entityList;
	if (fistCall)
		entityList = &(scene->GetRootEntities());
	else
		entityList = &(parentEntity.GetComponent<FamilyComponent>().Children);

	for (auto entity : *(entityList))
	{
		ImGuiTreeNodeFlags node_flags = base_flags;
		const bool is_selected = (*selection_mask & BIT(entity.GetID())) != 0;
		if (is_selected)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		bool entityIsParent = entity.GetComponent<FamilyComponent>().Children.size();

		if (!entityIsParent)
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		const char* name = entity.GetComponent<NameComponent>().Name.c_str();
		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)(entity.GetID()), node_flags, name);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10.0f, 10.0f });
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("_SCENE_GRAPH_ENTITY_DRAG", &entity, sizeof(Entity));
			ImGui::TextUnformatted(name);
			ImGui::EndDragDropSource();
		}
		ImGui::PopStyleVar();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE_GRAPH_ENTITY_DRAG"))
			{
				Entity* otherEntity = static_cast<Entity*>(payload->Data);
				otherEntity->SetParent(entity);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemClicked())
		{
			selectedEntity = entity;
		}

		if (entityIsParent)
		{
			if (node_open)
			{
				auto clickState = SceneGraphTreeViewRecursive(entity, selection_mask);

				if (!selectedEntity)
				{
					selectedEntity = clickState;
				}

				ImGui::TreePop();
			}
		}
	}

	return selectedEntity;
}

void Editor::OnSceneGraphGUI()
{
	if (m_OpenScene)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
		ImGui::Begin("Scene Graph");

		static int selection_mask = 0;

		Entity clickedEntity = SceneGraphTreeViewRecursive(Entity(), &selection_mask, true, m_OpenScene);

		if (clickedEntity)
		{
			// Update selection state
			// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
			if (ImGui::GetIO().KeyCtrl)
			{
				selection_mask ^= BIT(clickedEntity.GetID());          // CTRL+click to toggle
				auto it = std::find(m_SelectedEntities.begin(), m_SelectedEntities.end(), clickedEntity);
				if (it != m_SelectedEntities.end())
					m_SelectedEntities.erase(it);
				else
					m_SelectedEntities.push_back(clickedEntity);
			}
			else
			{
				selection_mask = BIT(clickedEntity.GetID());           // Click to single-select
				m_SelectedEntities.clear();
				m_SelectedEntities.push_back(clickedEntity);
			}
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 5.0f, 5.0f });
		if (ImGui::BeginPopupContextWindow("_SceneGraphContextMenu"))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				Entity createdEntity = m_OpenScene->CreateEntity();
				m_SelectedEntities.clear();
				m_SelectedEntities.push_back(createdEntity);
				selection_mask = BIT(createdEntity.GetID());
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();

		ImGui::BeginChild("Root Drag Drop Target");
		ImGui::EndChild();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE_GRAPH_ENTITY_DRAG"))
			{
				Entity* otherEntity = static_cast<Entity*>(payload->Data);
				otherEntity->ClearParent();
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}
	else
	{
		ImGui::Begin("Scene Graph");

		const char* text = "Please open a scene to start editing!";

		ImVec2 textSize = ImGui::CalcTextSize(text);
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		ImGui::Text(text);

		ImGui::End();
	}
}

template<typename T>
static inline void CheckComponentEditor(uint32_t componentTypeID, Entity entity)
{
	if (componentTypeID == entt::type_info<T>::id())
	{
		ComponentEditor<T>(entity, componentTypeID);
		ImGui::Separator();
	}
}

void Editor::OnInspectorGUI()
{
	ImGui::Begin("Inspector");

	if (m_SelectedEntities.size() == 1)
	{
		auto& selectedEntity = m_SelectedEntities[0];

		char id[100];
		sprintf_s(id, sizeof(id) / sizeof(char), "INSPECTOR_ENTITY_EDITOR%i", selectedEntity.GetID());
		ImGui::PushID(id);

		auto& name = selectedEntity.GetComponent<NameComponent>();
		ImGui::InputText("Name", &name.Name);

		char idText[15];
		sprintf_s(idText, sizeof(idText) / sizeof(char), "ID: %i", selectedEntity.GetID());
		ImGui::TextUnformatted(idText);

		ImGui::SameLine();

		bool wannaDestroy = false;
		if (ImGui::SmallButton("Destroy Entity"))
			wannaDestroy = true;

		ImGui::Separator();

		for (const auto& componentTypeID : selectedEntity.GetEntitiesComponentsTypeIDList())
		{
			CheckComponentEditor<TransformComponent>(componentTypeID, selectedEntity);
			CheckComponentEditor<SpriteRendererComponent>(componentTypeID, selectedEntity);
			CheckComponentEditor<CameraComponent>(componentTypeID, selectedEntity);
		}

		if (ImGui::Button("Add Component##Button", ImVec2(150.0f, 30.0f)))
			ImGui::OpenPopup("Add Component##Popup");

		if (ImGui::BeginPopup("Add Component##Popup"))
		{
			if (!selectedEntity.HasComponent<TransformComponent>() && ImGui::Selectable("Transform Component##AddComponentPopup"))
				selectedEntity.AddComponent<TransformComponent>();

			if (!selectedEntity.HasComponent<SpriteRendererComponent>() && ImGui::Selectable("SpriteRenderer Component##AddComponentPopup"))
				selectedEntity.AddComponent<SpriteRendererComponent>(nullptr);

			if (!selectedEntity.HasComponent<CameraComponent>() && ImGui::Selectable("Camera Component##AddComponentPopup"))
				selectedEntity.AddComponent<CameraComponent>(Camera(CameraType::Orthographic, 10.0f, 1.0f, -1.0f, 1.0f));

			ImGui::EndPopup();
		}

		ImGui::PopID();

		if (wannaDestroy)
		{
			selectedEntity.Destroy();
			m_SelectedEntities.clear();
		}
	}
	else if (m_SelectedEntities.size() > 1)
	{
		ImGui::TextUnformatted("Cannot edit multiple entities at the same time!");
	}
	else
	{
		ImGui::TextUnformatted("Please select some thing to edit!");
	}

	ImGui::End();
}

/////////////////////////////////////////////
// Assets ///////////////////////////////////
/////////////////////////////////////////////

#if 0
static TripleBinding<bool, uint32_t, String> DirectoryTreeViewRecursive(Editor* editor, const std::filesystem::path& path, uint32_t* count, int* selection_mask, bool drawFiles)
{
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

	bool any_node_clicked = false;
	uint32_t node_clicked = 0;
	String clickedNodePath;
	String pathString, name, format;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		pathString = entry.path().string();
		name = FileSystem::ExtractFileNameFromPath(pathString);
		bool entryIsFile = FileSystem::IsFile(entry.path());

		if (entryIsFile)
		{
			if (!drawFiles)
				continue;

			format = FileSystem::ExtractFileExtentionFromName(name);

			if (format == OE_RESOURCE_DEFENITION_FILE_EXTENSION)
			{
				(*count)--;
				continue;
			}
		}

		ImGuiTreeNodeFlags node_flags = base_flags;
		const bool is_selected = (*selection_mask & BIT(*count)) != 0;
		if (is_selected)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		if (entryIsFile)
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)(*count), node_flags, name.c_str());

		if (ImGui::IsItemClicked())
		{
			if (entryIsFile && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (format == OE_SCENE_FILE_EXTENSION)
					editor->EditScene(LoadSceneFromFile(pathString), pathString);
			}

			node_clicked = *count;
			any_node_clicked = true;
			clickedNodePath = pathString;
		}

		(*count)--;

		if (!entryIsFile)
		{
			if (node_open)
			{
				auto clickState = DirectoryTreeViewRecursive(editor, entry.path(), count, selection_mask, drawFiles);

				if (!any_node_clicked && clickState.first)
				{
					any_node_clicked = clickState.first;
					node_clicked = clickState.middle;
					clickedNodePath = clickState.last;
				}

				ImGui::TreePop();
			}
			else
			{
				for (const auto& e : std::filesystem::recursive_directory_iterator(entry.path()))
					(*count)--;
			}
		}
	}

	return { any_node_clicked, node_clicked, clickedNodePath };
}

void Editor::OnAssetsGUI()
{
	bool is_root_clicked = false;
	static int selection_mask = 0;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::Begin("Assets");
	ImGui::PopStyleVar();

	if (!m_AssetBrowserOneColumnView)
		ImGui::Columns(2);

	if (ImGui::Button("Settings"))
		ImGui::OpenPopup("SettingsPopup##AssetBrowser");
	if (ImGui::BeginPopup("SettingsPopup##AssetBrowser"))
	{
		ImGui::Checkbox("One Column View", &m_AssetBrowserOneColumnView);
		ImGui::EndPopup();
	}

	ImGuiTreeNodeFlags root_node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

	if (m_IsAssetRootSelected)
		root_node_flags |= ImGuiTreeNodeFlags_Selected;
	
	if (ImGui::TreeNodeEx("##AssetsRootTreeNode", root_node_flags, "assets://"))
	{
		if (ImGui::IsItemClicked())
			is_root_clicked = true;

		String assetsPath = m_EditingProject->GetAssetsDirectoryPath();

		uint32_t count = 0;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath))
			count++;


		auto clickState = DirectoryTreeViewRecursive(this, assetsPath, &count, &selection_mask, m_AssetBrowserOneColumnView);

		if (is_root_clicked)
		{
			if (ImGui::GetIO().KeyCtrl)
			{
				m_IsAssetRootSelected = !m_IsAssetRootSelected;
			}
			else
			{
				m_IsAssetRootSelected = true;
				selection_mask = 0;
				m_SelectedAssetFiles.clear();
			}
		}
		else if (clickState.first)
		{
			// Update selection state
			// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
			if (ImGui::GetIO().KeyCtrl)
			{
				selection_mask ^= BIT(clickState.middle);          // CTRL+click to toggle
				auto it = std::find(m_SelectedAssetFiles.begin(), m_SelectedAssetFiles.end(), clickState.last);
				if (it != m_SelectedAssetFiles.end())
					m_SelectedAssetFiles.erase(it);
				else
					m_SelectedAssetFiles.push_back(clickState.last);
			}
			else
			{
				selection_mask = BIT(clickState.middle);           // Click to single-select
				m_IsAssetRootSelected = false;
				m_SelectedAssetFiles.clear();
				m_SelectedAssetFiles.push_back(clickState.last);
			}
		}

		ImGui::TreePop();
	}

	if (!m_AssetBrowserOneColumnView)
		ImGui::NextColumn();

	if (!m_AssetBrowserOneColumnView)
	{
		ImGui::PushItemWidth(-1);
		ImGui::SliderInt("##Thumbnail Size", &m_AssetThumbnailSize, m_AssetThumbnailSizeMin, m_AssetThumbnailSizeMax, "Thumbnail Size : %d px");
		ImGui::PopItemWidth();

		if (m_SelectedAssetFiles.size() > 1 || m_SelectedAssetFiles.size() == 1 && m_IsAssetRootSelected)
			ImGui::TextUnformatted("Showing multiple files and folders");
		else if (m_SelectedAssetFiles.size() == 1)
			ImGui::TextUnformatted(FileSystem::FixFileSystemPath(m_SelectedAssetFiles[0]).c_str());
		else
			ImGui::TextUnformatted(FileSystem::FixFileSystemPath(m_EditingProject->GetAssetsDirectoryPath()).c_str());

		ImGui::BeginChild("##ThumbnailView", { 0, 0 }, false, ImGuiWindowFlags_AlwaysUseWindowPadding);

		uint32_t count = 0;
		for (const auto& p : m_SelectedAssetFiles)
		{
			if (FileSystem::IsDirectory(p))
				for (const auto& entry : std::filesystem::directory_iterator(p))
					count++;
			else
				count++;
		}

		ImVec2 thumbnailSize((float)m_AssetThumbnailSize, (float)m_AssetThumbnailSize);
		ImGuiStyle& style = ImGui::GetStyle();
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

		uint32_t n = 0;
		if (m_IsAssetRootSelected)
		{
			auto assetsPath = m_EditingProject->GetAssetsDirectoryPath();

			for (const auto& entry : std::filesystem::directory_iterator(assetsPath))
				count++;

			for (const auto& entry : std::filesystem::directory_iterator(assetsPath))
			{
				ImGui::PushID(n);

				String name = FileSystem::ExtractFileNameFromPath(entry.path().string());

				ImGui::Button(name.c_str(), thumbnailSize);
				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + thumbnailSize.x; // Expected position if next button was on same line
				if (n + 1 < count && next_button_x2 < window_visible_x2)
					ImGui::SameLine();
				ImGui::PopID();
				n++;
			}
		}

		for (const auto& p : m_SelectedAssetFiles)
		{
			if (FileSystem::IsDirectory(p))
			{
				for (const auto& entry : std::filesystem::directory_iterator(p))
				{
					ImGui::PushID(n);

					String name = FileSystem::ExtractFileNameFromPath(entry.path().string());

					ImGui::Button(name.c_str(), thumbnailSize);
					float last_button_x2 = ImGui::GetItemRectMax().x;
					float next_button_x2 = last_button_x2 + style.ItemSpacing.x + thumbnailSize.x; // Expected position if next button was on same line
					if (n + 1 < count && next_button_x2 < window_visible_x2)
						ImGui::SameLine();
					ImGui::PopID();
					n++;
				}
			}
		}
		ImGui::EndChild();
	}

	if (!m_AssetBrowserOneColumnView)
		ImGui::Columns(1);

	ImGui::End();
}
#endif

static TripleBinding<bool, uint32_t, String> DirectoryTreeViewRecursive(Editor* editor, const std::filesystem::path& path, uint32_t* count, int* selection_mask, bool drawFiles)
{
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

	bool any_node_clicked = false;
	uint32_t node_clicked = 0;
	String clickedNodePath;
	String pathString, name, format;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		pathString = entry.path().string();
		name = FileSystem::ExtractFileNameFromPath(pathString);
		bool entryIsFile = FileSystem::IsFile(entry.path());

		if (entryIsFile)
		{
			if (!drawFiles)
				continue;

			format = FileSystem::ExtractFileExtentionFromName(name);

			if (format == OE_RESOURCE_DEFENITION_FILE_EXTENSION)
			{
				(*count)--;
				continue;
			}
		}

		ImGuiTreeNodeFlags node_flags = base_flags;
		const bool is_selected = (*selection_mask & BIT(*count)) != 0;
		if (is_selected)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		if (entryIsFile)
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)(*count), node_flags, name.c_str());

		if (ImGui::IsItemClicked())
		{
			if (entryIsFile && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (format == OE_SCENE_FILE_EXTENSION)
					editor->EditScene(LoadSceneFromFile(pathString), pathString);
			}

			node_clicked = *count;
			any_node_clicked = true;
			clickedNodePath = pathString;
		}

		(*count)--;

		if (!entryIsFile)
		{
			if (node_open)
			{
				auto clickState = DirectoryTreeViewRecursive(editor, entry.path(), count, selection_mask, drawFiles);

				if (!any_node_clicked && clickState.first)
				{
					any_node_clicked = clickState.first;
					node_clicked = clickState.middle;
					clickedNodePath = clickState.last;
				}

				ImGui::TreePop();
			}
			else
			{
				for (const auto& e : std::filesystem::recursive_directory_iterator(entry.path()))
					(*count)--;
			}
		}
	}

	return { any_node_clicked, node_clicked, clickedNodePath };
}

void Editor::OnAssetsGUI()
{
	bool is_root_clicked = false;
	static int selection_mask = 0;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::Begin("Assets");
	ImGui::PopStyleVar();

	if (!m_AssetBrowserOneColumnView)
		ImGui::Columns(2);

	if (ImGui::Button("Settings"))
		ImGui::OpenPopup("SettingsPopup##AssetBrowser");
	if (ImGui::BeginPopup("SettingsPopup##AssetBrowser"))
	{
		ImGui::Checkbox("One Column View", &m_AssetBrowserOneColumnView);
		ImGui::EndPopup();
	}

	ImGuiTreeNodeFlags root_node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

	if (m_IsAssetRootSelected)
		root_node_flags |= ImGuiTreeNodeFlags_Selected;

	if (ImGui::TreeNodeEx("##AssetsRootTreeNode", root_node_flags, "assets://"))
	{
		if (ImGui::IsItemClicked())
			is_root_clicked = true;

		String assetsPath = m_EditingProject->GetAssetsDirectoryPath();

		uint32_t count = 0;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath))
			count++;


		auto clickState = DirectoryTreeViewRecursive(this, assetsPath, &count, &selection_mask, m_AssetBrowserOneColumnView);

		if (is_root_clicked)
		{
			if (ImGui::GetIO().KeyCtrl)
			{
				m_IsAssetRootSelected = !m_IsAssetRootSelected;
			}
			else
			{
				m_IsAssetRootSelected = true;
				selection_mask = 0;
				m_SelectedAssetFiles.clear();
			}
		}
		else if (clickState.first)
		{
			// Update selection state
			// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
			if (ImGui::GetIO().KeyCtrl)
			{
				selection_mask ^= BIT(clickState.middle);          // CTRL+click to toggle
				auto it = std::find(m_SelectedAssetFiles.begin(), m_SelectedAssetFiles.end(), clickState.last);
				if (it != m_SelectedAssetFiles.end())
					m_SelectedAssetFiles.erase(it);
				else
					m_SelectedAssetFiles.push_back(clickState.last);
			}
			else
			{
				selection_mask = BIT(clickState.middle);           // Click to single-select
				m_IsAssetRootSelected = false;
				m_SelectedAssetFiles.clear();
				m_SelectedAssetFiles.push_back(clickState.last);
			}
		}

		ImGui::TreePop();
	}

	if (!m_AssetBrowserOneColumnView)
		ImGui::NextColumn();

	if (!m_AssetBrowserOneColumnView)
	{
		ImGui::PushItemWidth(-1);
		ImGui::SliderInt("##Thumbnail Size", &m_AssetThumbnailSize, m_AssetThumbnailSizeMin, m_AssetThumbnailSizeMax, "Thumbnail Size : %d px");
		ImGui::PopItemWidth();

		if (m_SelectedAssetFiles.size() > 1 || m_SelectedAssetFiles.size() == 1 && m_IsAssetRootSelected)
			ImGui::TextUnformatted("Showing multiple files and folders");
		else if (m_SelectedAssetFiles.size() == 1)
			ImGui::TextUnformatted(FileSystem::FixFileSystemPath(m_SelectedAssetFiles[0]).c_str());
		else
			ImGui::TextUnformatted(FileSystem::FixFileSystemPath(m_EditingProject->GetAssetsDirectoryPath()).c_str());

		ImGui::BeginChild("##ThumbnailView", { 0, 0 }, false, ImGuiWindowFlags_AlwaysUseWindowPadding);

		uint32_t count = 0;
		for (const auto& p : m_SelectedAssetFiles)
		{
			if (FileSystem::IsDirectory(p))
				for (const auto& entry : std::filesystem::directory_iterator(p))
					count++;
			else
				count++;
		}

		ImVec2 thumbnailSize((float)m_AssetThumbnailSize, (float)m_AssetThumbnailSize);
		ImGuiStyle& style = ImGui::GetStyle();
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

		uint32_t n = 0;
		if (m_IsAssetRootSelected)
		{
			auto assetsPath = m_EditingProject->GetAssetsDirectoryPath();

			for (const auto& entry : std::filesystem::directory_iterator(assetsPath))
				count++;

			for (const auto& entry : std::filesystem::directory_iterator(assetsPath))
			{
				ImGui::PushID(n);

				String name = FileSystem::ExtractFileNameFromPath(entry.path().string());

				ImGui::Button(name.c_str(), thumbnailSize);
				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + thumbnailSize.x; // Expected position if next button was on same line
				if (n + 1 < count && next_button_x2 < window_visible_x2)
					ImGui::SameLine();
				ImGui::PopID();
				n++;
			}
		}

		for (const auto& p : m_SelectedAssetFiles)
		{
			if (FileSystem::IsDirectory(p))
			{
				for (const auto& entry : std::filesystem::directory_iterator(p))
				{
					ImGui::PushID(n);

					String name = FileSystem::ExtractFileNameFromPath(entry.path().string());

					ImGui::Button(name.c_str(), thumbnailSize);
					float last_button_x2 = ImGui::GetItemRectMax().x;
					float next_button_x2 = last_button_x2 + style.ItemSpacing.x + thumbnailSize.x; // Expected position if next button was on same line
					if (n + 1 < count && next_button_x2 < window_visible_x2)
						ImGui::SameLine();
					ImGui::PopID();
					n++;
				}
			}
		}
		ImGui::EndChild();
	}

	if (!m_AssetBrowserOneColumnView)
		ImGui::Columns(1);

	ImGui::End();
}

void Editor::OnAssetImportGUI()
{
	ImGui::Begin("Asset Import");

	if (m_SelectedAssetFiles.size() == 1)
	{
		String& assetPath = m_SelectedAssetFiles[0];

		if (FileSystem::IsFile(assetPath))
		{
			ImGui::Text("Editing Resource : %s", FileSystem::ExtractFileNameFromPath(assetPath).c_str());

			if (!FileSystem::FileExists(assetPath + "." + OE_RESOURCE_DEFENITION_FILE_EXTENSION))
			{
				auto name = FileSystem::ExtractFileNameFromPath(assetPath);
				auto format = FileSystem::ExtractFileExtentionFromName(name);

				if (format == OE_SCENE_FILE_EXTENSION)
				{
					ImGui::TextUnformatted("OverEngine Scene File");
				}
				else
				{
					ImGui::TextWrapped("No 'ResourceDefenition' found for this file. OverEngine should make one for files that recognized as Resources. If not; It is not a supported file. but you can create that manually.");

					/*if (ImGui::Button("Create ResourceDefenition"))
					{
						std::ofstream manifestFile((assetPath + "." + OE_RESOURCE_DEFENITION_FILE_EXTENSION).c_str());
						manifestFile << "{\n\t\"Assets\": []\n}";
						manifestFile.flush();
						manifestFile.close();
					}*/
				}
			}
		}
	}

	ImGui::End();
}

void Editor::OnConsoleGUI()
{
	ImGui::Begin("Console");

	// ImGui::Text("Logs Here!");
	ImGuiStyle& style = ImGui::GetStyle();
	const float spacing = style.ItemInnerSpacing.x;

	// Text change level
	static bool a, b, c;
	ImGui::Checkbox("Error", &a); ImGui::SameLine();
	ImGui::Checkbox("Warning", &b); ImGui::SameLine();
	ImGui::Checkbox("Info", &c);

	ImGui::SameLine(0.0f, spacing);

	// Button for advanced settings
	ImGui::SameLine(0.0f, ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Settings").x - style.WindowPadding.x / 2.0f);
	if (ImGui::Button("Settings"))
		ImGui::OpenPopup("SettingsPopup");
	if (ImGui::BeginPopup("SettingsPopup"))
	{
		ImGui::EndPopup();
	}


	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
	ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	{
		/*ImGui::SetWindowFontScale(s_DisplayScale);

		auto messageStart = s_MessageBuffer.begin() + s_MessageBufferBegin;
		if (*messageStart) // If contains old message here
			for (auto message = messageStart; message != s_MessageBuffer.end(); message++)
				(*message)->OnImGuiRender();
		if (s_MessageBufferBegin != 0) // Skipped first messages in vector
			for (auto message = s_MessageBuffer.begin(); message != messageStart; message++)
				(*message)->OnImGuiRender();

		if (s_RequestScrollToBottom && ImGui::GetScrollMaxY() > 0)
		{
			ImGui::SetScrollY(ImGui::GetScrollMaxY());
			s_RequestScrollToBottom = false;
		}*/
	}
	ImGui::EndChild();
	ImGui::PopStyleColor();

	ImGui::End();
}
