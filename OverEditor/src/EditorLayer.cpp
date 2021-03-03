#include "EditorLayer.h"

#include "UI/EditorConsoleSink.h"

#include <OverEngine/Core/FileSystem/FileSystem.h>
#include <OverEngine/Scene/SceneSerializer.h>
#include <OverEngine/Core/Extensions.h>

#include <imgui/imgui.h>
#include <tinyfiledialogs/tinyfiledialogs.h>

#include <OverEngine/ImGui/DockingLayout.h>

namespace OverEditor
{
	EditorLayer* EditorLayer::s_Instance = nullptr;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
		s_Instance = this;
		Application::Get().GetImGuiLayer()->SetBlockEvents(false);
	}

	void EditorLayer::OnAttach()
	{
		OE_PROFILE_FUNCTION();

		// auto editorConsoleSink = std::make_shared<EditorConsoleSink_mt>(&m_ConsolePanel);
		// editorConsoleSink->set_pattern("%v");
		// Log::GetCoreLogger()->sinks().push_back(editorConsoleSink);
		// Log::GetClientLogger()->sinks().push_back(editorConsoleSink);

		m_SceneContext = CreateRef<SceneEditor>();
		m_SceneContext->PrimaryScene = nullptr;
		m_ViewportPanel.SetContext(m_SceneContext);
		m_SceneHierarchyPanel.SetContext(m_SceneContext);

		m_IconsTexture = Texture2D::Create("assets/textures/Icons.png");
		m_Icons["FolderIcon"] = SubTexture2D::Create(m_IconsTexture, { 256, 0, 256, 256 });
		m_Icons["SceneIcon"] = SubTexture2D::Create(m_IconsTexture, { 0, 0, 256, 256 });
	}

	void EditorLayer::OnUpdate(TimeStep deltaTime)
	{
		OE_PROFILE_FUNCTION();

		m_ViewportPanel.OnRender();

		if (m_EditingProject)
		{
			/*std::lock_guard<std::mutex> lock(m_EditingProject->GetAssetLoadCommandBufferMutex());

			auto& cmdBuffer = m_EditingProject->GetAssetLoadCommandBuffer();
			for (const auto& path : cmdBuffer)
				AssetManager::ImportAndLoad(path, m_EditingProject->GetAssetsDirectoryPath(), &m_EditingProject->GetAssets());

			cmdBuffer.clear();*/

			m_EditingProject->OnUpdate(deltaTime);
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		OE_PROFILE_FUNCTION();

		// Main Menubar
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project", "Ctrl+Shift+N"))
					m_IsProjectManagerOpen = true;

				if (ImGui::MenuItem("Open Project", "Ctrl+Shift+O"))
					m_IsProjectManagerOpen = true;

				ImGui::Separator();

				if (ImGui::MenuItem("Save Scene", "Ctrl+S", nullptr, m_SceneContext->AnySceneOpen()))
				{
					SceneSerializer sceneSerializer(m_SceneContext->PrimaryScene);
					sceneSerializer.Serialize(m_EditingProject->GetAssetsDirectoryPath() + static_cast<String>(m_SceneContext->PrimaryScene->GetPath()));
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Quit Editor", "Alt+F4"))
					Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
		ImGui::PopStyleVar();

		// DockSpace
		static constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(3);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			static ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
			static DockingLayout dl;
			static bool layoutLoaded = false;

			ImGui::Dummy({ -1, 5 });

			ImGui::Dummy({ 5, 1 });
			ImGui::SameLine();

			if (ImGui::Button("Save Layout"))
			{
				dl.Read(dockspace_id);
				dl.Save("Layout.yaml");
			}

			ImGui::SameLine();

			if (ImGui::Button("Load Layout") || !layoutLoaded)
			{
				layoutLoaded = true;

				dl.Load("Layout.yaml");
				dl.Apply(dockspace_id);
			}

			ImGui::Spacing();

			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		}

		ImGui::End();

		OnProjectManagerGUI();

		//m_ConsolePanel.OnImGuiRender();

		if (m_EditingProject)
		{
			m_ViewportPanel.OnImGuiRender();
			m_SceneHierarchyPanel.OnImGuiRender();
			m_AssetsPanel.OnImGuiRender();

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

	void EditorLayer::OnEvent(Event& event)
	{
		OE_PROFILE_FUNCTION();

		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& event) -> bool {

			bool control = Input::IsKeyPressed(KeyCode::LeftControl) || Input::IsKeyPressed(KeyCode::RightControl);
			bool shift = Input::IsKeyPressed(KeyCode::LeftShift) || Input::IsKeyPressed(KeyCode::RightShift);

			if (event.GetKeyCode() == KeyCode::Z && control)
			{
				if (shift)
					m_ActionStack.Redo();
				else
					m_ActionStack.Undo();
			}

			return false;

		});

		if (m_EditingProject)
		{
			dispatcher.Dispatch<WindowFocusEvent>([this](WindowFocusEvent& event) -> bool {

				AssetDatabase::Refresh();
				return false;

			});
		}
	}

	void EditorLayer::EditScene(const Ref<Scene>& scene)
	{
		m_SceneContext->PrimaryScene = scene;
		m_SceneContext->Selection.clear();

		char buf[128];
		sprintf_s(buf, OE_ARRAY_SIZE(buf), "OverEditor - %s - %s", m_EditingProject->GetName().c_str(), scene->GetName().c_str());
		Application::Get().GetWindow().SetTitle(buf);
	}

	void EditorLayer::OnProjectManagerGUI()
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
					extension << "*." << Extensions::ProjectFileExtension;
					const char* filters[] = { extension.str().c_str() };
					if (char* filePath = const_cast<char*>(tinyfd_openFileDialog("Open Project", "", 1, filters, "OverEngine Project", 0)))
					{
						FileSystem::FixPath(filePath);

						m_EditingProject = nullptr;
						auto project = CreateRef<EditorProject>(filePath);
						m_EditingProject = project;

						char buf[128];
						sprintf_s(buf, OE_ARRAY_SIZE(buf), "OverEditor - %s", project->GetName().c_str());
						Application::Get().GetWindow().SetTitle(buf);
					}
				}

				ImGui::Separator();

				if (ImGui::Button("Open Test Project"))
				{
					m_EditingProject = nullptr;
				#ifdef OE_PLATFORM_WINDOWS
					auto project = CreateRef<EditorProject>("D:/overenginedev/SuperMario/project.oep");
				#elif defined(OE_PLATFORM_LINUX)
					auto project = CreateRef<EditorProject>("/home/sepehr/dev/SuperMario/project.oep");
				#endif
					m_EditingProject = project;

					char buf[128];
					sprintf_s(buf, OE_ARRAY_SIZE(buf), "OverEditor - %s", project->GetName().c_str());
					Application::Get().GetWindow().SetTitle(buf);
				}

				/*ImGui::Separator();

				if (m_EditingProject && ImGui::Button("Create Scene"))
				{
					char buffer[16];
					sprintf_s(buffer, OE_ARRAY_SIZE(buffer), "*.%s", OE_SCENE_FILE_EXTENSION);

					const char* filters[] = { buffer };

					if (auto scenePath = tinyfd_saveFileDialog("Create Scene", "", OE_ARRAY_SIZE(filters), filters, "OverEngine Scene"))
					{
						auto scene = CreateRef<Scene>();
						SceneSerializer(scene).Serialize(scenePath);
						EditScene(scene, scenePath);
					}
				}*/
			}

			ImGui::End();
		}
	}
}
