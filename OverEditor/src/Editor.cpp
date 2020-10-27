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

namespace OverEditor
{
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
		m_Assets.InitFromAssetsDirectory(m_AssetsDirectoryPath, projectJson["AssetsRootGuid"].get<String>());

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
		: m_AssetsPanel(this)
	{
		m_SceneContext = CreateRef<SceneEditor>();
		m_ViewportPanel.SetContext(m_SceneContext);
		m_SceneHierarchyPanel.SetContext(m_SceneContext);

		m_IconsTexture = Texture2D::CreateMaster("assets/textures/Icons.png");
		m_Icons["FolderIcon"] = Texture2D::CreateSubTexture(m_IconsTexture, { 256, 0, 256, 256 });
		m_Icons["SceneIcon"] = Texture2D::CreateSubTexture(m_IconsTexture, { 0, 0, 256, 256 });
	}

	void Editor::OnUpdate()
	{
		m_ViewportPanel.OnRender();
	}

	void Editor::OnImGuiRender()
	{
		OnMainMenubarGUI();
		OnMainDockSpaceGUI();

		OnProjectManagerGUI();
		OnToolbarGUI();

		m_ConsolePanel.OnImGuiRender();

		if (m_EditingProject)
		{
			m_ViewportPanel.OnImGuiRender();
			m_SceneHierarchyPanel.OnImGuiRender();
			m_AssetsPanel.OnImGuiRender();

			OnInspectorGUI();

			ImGui::Begin("Renderer2D");
			ImGui::Columns(2);
			ImGui::TextUnformatted(Renderer2D::GetShader()->GetName().c_str());
			ImGui::NextColumn();
			if (ImGui::Button("Reload"))
				Renderer2D::GetShader()->Reload();
			ImGui::Columns(1);
			ImGui::End();
		}
	}

	void Editor::EditScene(const Ref<Scene>& scene, String path)
	{
		m_SceneContext->Context = scene;
		m_SceneContext->ContextResourcePath = path;
		m_SceneContext->SelectionContext.clear();

		char buf[128];
		sprintf_s(buf, OE_ARRAY_SIZE(buf), "OverEditor - %s - %s", m_EditingProject->GetName().c_str(), FileSystem::ExtractFileNameFromPath(path).c_str());
		Application::Get().GetMainWindow().SetTitle(buf);
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
		static constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1));
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(3);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
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
							m_EditingProject = project;
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
						m_EditingProject = project;

						char buf[128];
						sprintf_s(buf, OE_ARRAY_SIZE(buf), "OverEditor - %s", project->GetName().c_str());
						Application::Get().GetMainWindow().SetTitle(buf);
					}
				}

				ImGui::Separator();

				if (ImGui::Button("Open Test Project"))
				{
					auto project = CreateRef<EditorProject>("D:/overenginedev/SuperMario/project.oep");
					m_EditingProject = project;

					char buf[128];
					sprintf_s(buf, OE_ARRAY_SIZE(buf), "OverEditor - %s", project->GetName().c_str());
					Application::Get().GetMainWindow().SetTitle(buf);
				}

				ImGui::Separator();

				if (m_EditingProject && ImGui::Button("Create Scene"))
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

			ImGui::End();
		}
	}

	void Editor::OnToolbarGUI()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 8.0f, 4.0f });
		ImGui::Begin("Toolbar");
		ImGui::PopStyleVar();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 8.0f, 5.0f });

		if (ImGui::Button("New Project"))
			m_IsProjectManagerOpen = true;

		ImGui::SameLine();

		if (ImGui::Button("Open Project"))
			m_IsProjectManagerOpen = true;

		ImGui::SameLine();

		if (m_SceneContext->Context && ImGui::Button("Save Scene"))
		{
			auto pathToSave = m_EditingProject->GetAssetsDirectoryPath() + m_SceneContext->ContextResourcePath;
			m_SceneContext->Context->Dump(pathToSave);
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}

	template<typename T>
	static void CheckComponentEditor(uint32_t componentTypeID, Entity entity)
	{
		if (componentTypeID == entt::type_info<T>::id())
		{
			ComponentEditor<T>(entity, componentTypeID);
			ImGui::Separator();
		}
	}

	template<typename T, typename... Args>
	static void CheckAddComponent(Entity& entity, const char* label, Args&&... args)
	{
		if (!entity.HasComponent<T>() && ImGui::Selectable(label))
			entity.AddComponent<T>(std::forward<Args>(args)...);
	}

	void Editor::OnInspectorGUI()
	{
		ImGui::Begin("Inspector");

		if (m_SceneContext->SelectionContext.size() == 1)
		{
			auto& selectedEntity = m_SceneContext->SelectionContext[0];

			char id[100];
			sprintf_s(id, sizeof(id) / sizeof(char), "INSPECTOR_ENTITY_EDITOR%i", (uint32_t)selectedEntity);
			ImGui::PushID(id);

			auto& base = selectedEntity.GetComponent<BaseComponent>();
			ImGui::InputText("##Name", &base.Name);

			bool wannaDestroy = false;
			if (ImGui::SmallButton("Destroy Entity"))
				wannaDestroy = true;

			ImGui::SameLine();

			ImGui::TextWrapped("GUID: %s", base.ID.ToString().c_str());

			ImGui::Separator();

			for (const auto& componentTypeID : selectedEntity.GetComponentsTypeIDList())
			{
				CheckComponentEditor<TransformComponent>(componentTypeID, selectedEntity);
				CheckComponentEditor<SpriteRendererComponent>(componentTypeID, selectedEntity);
				CheckComponentEditor<CameraComponent>(componentTypeID, selectedEntity);
			}

			if (ImGui::Button("Add Component##Button", ImVec2(-1.0f, 40.0f)))
				ImGui::OpenPopup("Add Component##Popup");

			if (ImGui::BeginPopup("Add Component##Popup"))
			{
				CheckAddComponent<TransformComponent>(selectedEntity, "Transform Component##AddComponentPopup");
				CheckAddComponent<SpriteRendererComponent>(selectedEntity, "SpriteRenderer Component##AddComponentPopup", nullptr);
				CheckAddComponent<CameraComponent>(selectedEntity, "Camera Component##AddComponentPopup");

				ImGui::EndPopup();
			}

			ImGui::PopID();

			if (wannaDestroy)
			{
				selectedEntity.Destroy();
				m_SceneContext->SelectionContext.clear();
			}
		}
		else if (m_SceneContext->SelectionContext.size() > 1)
		{
			ImGui::TextUnformatted("Cannot edit multiple entities at the same time!");
		}
		else
		{
			ImGui::TextUnformatted("Please select some thing to edit!");
		}

		ImGui::End();
	}
}
