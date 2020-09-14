#include "Editor.h"

#include "ComponentEditor.h"

#include <OverEngine/ImGui/ExtraImGui.h>
#include <OverEngine/Core/FileSystem/FileSystem.h>
#include <OverEngine/Core/Extentions.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
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
	m_IconsTexture = Texture2D::MasterFromFile("assets/textures/Icons.png");
	m_Icons["FolderIcon"] = Texture2D::SubTextureFromExistingOne(m_IconsTexture, { 256, 0, 256, 256 });
	m_Icons["SceneIcon"] = Texture2D::SubTextureFromExistingOne(m_IconsTexture, { 0, 0, 256, 256 });
}

void Editor::OnUpdate()
{
	if (m_OpenScene)
		m_OpenScene->OnTransformUpdate();
	m_ViewportPanel.OnRender();
}

void Editor::OnImGuiRender()
{
	OnMainMenubarGUI();
	OnMainDockSpaceGUI();

	OnProjectManagerGUI();
	OnToolbarGUI();

	if (m_EditingProject)
	{
		m_ViewportPanel.OnImGuiRender();
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
			auto pathToSave = m_EditingProject->GetAssetsDirectoryPath() + "/" + m_OpenScenePath.substr(9, m_OpenScenePath.size());
			SaveSceneToFile(pathToSave, m_OpenScene);
		}
	}

	ImGui::PopStyleVar();

	ImGui::End();
	ImGui::PopStyleVar();
}

/////////////////////////////////////////////
// Scene ////////////////////////////////////
/////////////////////////////////////////////

static Entity SceneGraphTreeViewRecursive(Entity parentEntity, int* selection_mask, bool fistCall = false, Ref<Scene> scene = nullptr)
{
	ImGuiTreeNodeFlags base_flags = OE_IMGUI_BASE_TREE_VIEW_FLAGS;

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

		bool wannaDestroy = false;
		if (ImGui::SmallButton("Destroy Entity"))
			wannaDestroy = true;

		ImGui::SameLine();

		ImGui::Text("GUID: %s", selectedEntity.GetComponent<GUIDComponent>().ID.ToString().c_str());

		ImGui::Separator();

		for (const auto& componentTypeID : selectedEntity.GetEntitiesComponentsTypeIDList())
		{
			CheckComponentEditor<TransformComponent>(componentTypeID, selectedEntity);
			CheckComponentEditor<SpriteRendererComponent>(componentTypeID, selectedEntity);
			CheckComponentEditor<CameraComponent>(componentTypeID, selectedEntity);
		}

		if (ImGui::Button("Add Component##Button", ImVec2(-1.0f, 40.0f)))
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

static std::pair<bool, Ref<Resource>> ResourcesTreeViewRecursive(Editor* editor, const Ref<Resource>& resourceToDraw, Vector<Ref<Resource>>* selectedResources, bool drawNonDirectories)
{
	bool anyNodeClicked = false;
	Ref<Resource> nodeClicked;

	for (const auto& resource : resourceToDraw->GetChildren())
	{
		if (!drawNonDirectories && !resource->IsDirectory())
			continue;

		ImGuiTreeNodeFlags node_flags = OE_IMGUI_BASE_TREE_VIEW_FLAGS;
		if (std::find(selectedResources->begin(), selectedResources->end(), resource) != selectedResources->end())
			node_flags |= ImGuiTreeNodeFlags_Selected;

		if (!resource->IsDirectory())
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		bool nodeOpen = ImGui::TreeNodeEx(resource->GetGuid().ToString().c_str(), node_flags, resource->GetName().c_str());

		// EXTREAMLY TEMPO
		if (resource->GetType() == ResourceType::Texture2D)
		{
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::TextUnformatted(resource->GetName().c_str());
				ImGui::Image((*(Ref<Texture2DAsset>*) & resource->GetAssets()[0])->GetAsset(), { 128, 128 });
				ImGui::EndTooltip();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10.0f, 10.0f });
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("_ASSET_DRAG", &resource->GetAssets()[0], sizeof(Ref<Asset>));
				ImGui::TextUnformatted(resource->GetName().c_str());
				ImGui::Image((*(Ref<Texture2DAsset>*) & resource->GetAssets()[0])->GetAsset(), { 128, 128 });
				ImGui::EndDragDropSource();
			}
			ImGui::PopStyleVar();
		}

		if (ImGui::IsItemClicked())
		{
			if (!resource->IsDirectory() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (resource->GetType() == ResourceType::Scene && resource->GetAssets()[0]->GetType() == AssetType::Scene)
					editor->EditScene((*(Ref<SceneAsset>*)(&resource->GetAssets()[0]))->GetAsset(), resource->GetVirtualPath());
			}

			anyNodeClicked = true;
			nodeClicked = resource;
		}

		if (resource->IsDirectory())
		{
			if (nodeOpen)
			{
				auto clickState = ResourcesTreeViewRecursive(editor, resource, selectedResources, drawNonDirectories);

				if (!anyNodeClicked && clickState.first)
				{
					anyNodeClicked = true;
					nodeClicked = clickState.second;
				}

				ImGui::TreePop();
			}
		}
	}

	return { anyNodeClicked, nodeClicked };
}

void Editor::OnAssetsGUI()
{
	ImGui::Begin("Assets");

	if (!m_AssetBrowserOneColumnView)
		ImGui::Columns(2);

	if (ImGui::Button("Settings"))
		ImGui::OpenPopup("SettingsPopup##AssetBrowser");
	if (ImGui::BeginPopup("SettingsPopup##AssetBrowser"))
	{
		ImGui::Checkbox("One Column View", &m_AssetBrowserOneColumnView);
		ImGui::EndPopup();
	}

	const auto& rootResource = m_EditingProject->GetResources().GetResource("assets://");

	ImGuiTreeNodeFlags rootNodeFlags = OE_IMGUI_BASE_TREE_VIEW_FLAGS;
	if (std::find(m_SelectedResources.begin(), m_SelectedResources.end(), rootResource) != m_SelectedResources.end())
		rootNodeFlags |= ImGuiTreeNodeFlags_Selected;

	bool rootOpen = ImGui::TreeNodeEx("##AssetsRootTreeNode", rootNodeFlags, "assets://");
	std::pair<bool, Ref<Resource>> clickState{ false, nullptr };

	if (ImGui::IsItemClicked())
	{
		clickState.first = true;
		clickState.second = rootResource;
	}

	if (rootOpen)
	{
		auto childsClickState = ResourcesTreeViewRecursive(this, rootResource, &m_SelectedResources, m_AssetBrowserOneColumnView);
		if (childsClickState.first)
			clickState = childsClickState;
		ImGui::TreePop();
	}

	if (clickState.first)
	{
		if (ImGui::GetIO().KeyCtrl)
		{
			auto it = std::find(m_SelectedResources.begin(), m_SelectedResources.end(), clickState.second);
			if (it != m_SelectedResources.end())
				m_SelectedResources.erase(it);
			else
				m_SelectedResources.push_back(clickState.second);
		}
		else
		{
			m_SelectedResources.clear();
			m_SelectedResources.push_back(clickState.second);
		}
	}

	//////////////////////////////////////////////////////
	// Thumbnails ////////////////////////////////////////
	//////////////////////////////////////////////////////

	if (!m_AssetBrowserOneColumnView)
	{
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		ImGui::Spacing();
		ImGui::SliderInt("##Thumbnail Size", &m_AssetThumbnailSize, m_AssetThumbnailSizeMin, m_AssetThumbnailSizeMax, "Thumbnail Size : %d px");
		ImGui::PopItemWidth();

		if (m_SelectedResources.size() > 1)
			ImGui::TextUnformatted("Showing multiple files and folders");
		else if (m_SelectedResources.size() == 1)
		{
			if (ImGui::ArrowButton("AssetThumbnailBackButton", ImGuiDir_Left))
			{
				auto parent = m_SelectedResources[0]->GetParent();
				if (parent)
					m_SelectedResources[0] = m_SelectedResources[0]->GetParent();
			}
			ImGui::SameLine();
			ImGui::TextUnformatted(m_SelectedResources[0]->GetName().c_str());
		}

		ImGui::BeginChild("##ThumbnailView", { 0, 0 }, false, ImGuiWindowFlags_AlwaysUseWindowPadding);

		uint32_t count = 0;
		for (const auto& res : m_SelectedResources)
		{
			if (res->IsDirectory())
				count += (uint32_t)res->GetChildren().size();
			else
				count++;
		}

		ImVec2 thumbnailSize((float)m_AssetThumbnailSize, (float)m_AssetThumbnailSize);
		ImGuiStyle& style = ImGui::GetStyle();
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
		static const ImVec2 threeDotsTextSize = ImGui::CalcTextSize("...");

		uint32_t n = 0;
		for (const auto& res : m_SelectedResources)
		{
			if (!res->IsDirectory())
				continue;

			for (const auto& resource : res->GetChildren())
			{
				ImGui::PushID(n);
				if (resource->GetType() == ResourceType::Texture2D)
				{
					ImGui::ImageButton((*(Ref<Texture2DAsset>*) & resource->GetAssets()[0])->GetAsset(), thumbnailSize);

					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::TextUnformatted(resource->GetName().c_str());
						//ImGui::Image((*(Ref<Texture2DAsset>*) & resource->GetAssets()[0])->GetAsset(), { 128, 128 });
						ImGui::EndTooltip();
					}

					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10.0f, 10.0f });
					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("_ASSET_DRAG", &resource->GetAssets()[0], sizeof(Ref<Asset>));
						ImGui::TextUnformatted(resource->GetName().c_str());
						//ImGui::Image((*(Ref<Texture2DAsset>*) & resource->GetAssets()[0])->GetAsset(), { 128, 128 });
						ImGui::EndDragDropSource();
					}
					ImGui::PopStyleVar();

					ImVec2 rectMin = ImGui::GetItemRectMin();
					ImVec2 textSize = ImGui::CalcTextSize(resource->GetName().c_str());
					if (textSize.x + 10 <= thumbnailSize.x) // Enough space
					{
						ImVec2 textPos = { rectMin.x + (thumbnailSize.x - textSize.x) / 2, rectMin.y + thumbnailSize.y + 15 };
						windowDrawList->AddRectFilled({ textPos.x - 5, textPos.y }, { textPos.x + textSize.x + 5, textPos.y + textSize.y }, ImColor(20, 20, 20), 5);
						windowDrawList->AddText(textPos, ImColor(1.0f, 1.0f, 1.0f), resource->GetName().c_str());
					}
					else // Tight
					{
						ImVec2 posMax = { rectMin.x + thumbnailSize.x - 5, rectMin.y + thumbnailSize.y + 30 };
						windowDrawList->AddRectFilled({ rectMin.x, rectMin.y + thumbnailSize.y + 15 }, { posMax.x + 5, posMax.y }, ImColor(20, 20, 20), 5);
						ImGui::RenderTextEllipsis(windowDrawList, { rectMin.x + 5, rectMin.y + thumbnailSize.y + 15 }, posMax, posMax.x, posMax.x, resource->GetName().c_str(), nullptr, &textSize);
					}
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.2f, 0.2f, 0.2f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.2f, 0.2f, 0.5f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.2f, 0.2f, 0.7f });
					if (resource->GetType() == ResourceType::Directory)
						ImGui::ImageButton(m_Icons["FolderIcon"], thumbnailSize, -1, { 0.0f, 0.0f, 0.0f, 0.0f });
					else if (resource->GetType() == ResourceType::Scene)
						ImGui::ImageButton(m_Icons["SceneIcon"], thumbnailSize, -1, { 0.0f, 0.0f, 0.0f, 0.0f });
					else
						ImGui::Button("", thumbnailSize);
					ImGui::PopStyleColor(3);

					if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (resource->IsDirectory())
						{
							m_SelectedResources.clear();
							m_SelectedResources.push_back(resource);
						}
						else if (resource->GetType() == ResourceType::Scene && resource->GetAssets()[0]->GetType() == AssetType::Scene)
						{
							EditScene((*(Ref<SceneAsset>*)(&resource->GetAssets()[0]))->GetAsset(), resource->GetVirtualPath());
						}
					}

					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::TextUnformatted(resource->GetName().c_str());
						ImGui::EndTooltip();
					}

					ImVec2 rectMin = ImGui::GetItemRectMin();
					ImVec2 textSize = ImGui::CalcTextSize(resource->GetName().c_str());
					if (textSize.x + 10 <= thumbnailSize.x) // Enough space
					{
						ImVec2 textPos = { rectMin.x + (thumbnailSize.x - textSize.x) / 2, rectMin.y + thumbnailSize.y + 15 };
						windowDrawList->AddRectFilled({ textPos.x - 5, textPos.y }, { textPos.x + textSize.x + 5, textPos.y + textSize.y }, ImColor(20, 20, 20), 5);
						windowDrawList->AddText(textPos, ImColor(1.0f, 1.0f, 1.0f), resource->GetName().c_str());
					}
					else // Tight
					{
						ImVec2 posMax = { rectMin.x + thumbnailSize.x - 5, rectMin.y + thumbnailSize.y + 30 };
						windowDrawList->AddRectFilled({ rectMin.x, rectMin.y + thumbnailSize.y + 15 }, { posMax.x + 5, posMax.y }, ImColor(20, 20, 20), 5);
						ImGui::RenderTextEllipsis(windowDrawList, { rectMin.x + 5, rectMin.y + thumbnailSize.y + 15 }, posMax, posMax.x, posMax.x, resource->GetName().c_str(), nullptr, &textSize);
					}
				}

				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + thumbnailSize.x; // Expected position if next button was on same line
				if (n + 1 < count && next_button_x2 < window_visible_x2)
					ImGui::SameLine();
				else
					ImGui::Dummy({ 0, 25 });

				ImGui::PopID();
				n++;
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

	#if 0
	if (m_SelectedResources.size() == 1)
	{
		String& assetPath = m_SelectedResources[0];

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
	#endif

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
		ImGui::Text("WOW");
	}
	ImGui::EndChild();
	ImGui::PopStyleColor();

	ImGui::End();
}
