#include "pcheader.h"
#include "EditorLayer.h"

#include "imgui/imgui.h"
#include "OverEngine/Application.h"

namespace OverEditor
{
	EditorLayer::EditorLayer()
		: Layer("Editor Layer")
	{
	}

	void EditorLayer::OnUpdate()
	{
	}

	void EditorLayer::OnImGuiRender()
	{
		static char Name;
		static float Pos[3] = { 0, 0, 0 };
		static float Rot[3] = { 0, 0, 0 };
		static float Scl[3] = { 0, 0, 0 };
		MainMenuBar();
		MainDockSpace();

		ImGui::Begin("Test");
		ImGui::Text("I am rendering from client!");
		ImGui::Text("Dock other windows down here by dragging from title bar");
		ImGui::End();

		ImGui::Begin("Inspector");
		ImGui::InputText("Entity Name", &Name, 20 * sizeof(char));
		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::DragFloat3("Position", Pos);
			ImGui::DragFloat3("Rotation", Rot);
			ImGui::DragFloat3("Scale", Scl);
		}

		if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowSize().x, 45.0f)))
		{
		}

		ImGui::End();

		ImGui::Begin("Renderer");

		std::stringstream* ss = new std::stringstream();
		(*ss) << "Vendor: " << OverEngine::Application::Get().GetMainWindow().GetRendererContext()->GetInfoVendor();
		ImGui::Text(ss->str().c_str());
		delete ss;

		ss = new std::stringstream();
		(*ss) << "Renderer: " << OverEngine::Application::Get().GetMainWindow().GetRendererContext()->GetInfoRenderer();
		ImGui::Text(ss->str().c_str());
		delete ss;

		ss = new std::stringstream();
		(*ss) << "Version: " << OverEngine::Application::Get().GetMainWindow().GetRendererContext()->GetInfoVersion();
		ImGui::Text(ss->str().c_str());
		delete ss;

		ImGui::End();

		bool scene_open = true;
		if (scene_open)
		{
			ImGui::Begin("Scene", &scene_open, ImGuiWindowFlags_MenuBar);

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::Button("Move")) {}
				if (ImGui::Button("Rotate")) {}
				if (ImGui::Button("Scale")) {}

				ImGui::EndMenuBar();
			}


			static ImColor col1 = ImColor(1, 0, 0);
			static ImU32 col = ImU32(col1);
			ImGui::GetWindowDrawList()->AddRectFilled(
				ImVec2(ImGui::GetCursorScreenPos()),
				ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - 10,
					ImGui::GetCursorScreenPos().y + ImGui::GetWindowHeight() - 90), col);

			ImGui::End();
		}

		bool game_open = true;
		if (game_open)
		{
			ImGui::Begin("Game", &game_open);

			static ImColor col1 = ImColor(1, 0, 0);
			static ImU32 col = ImU32(col1);
			ImGui::GetWindowDrawList()->AddRectFilled(
				ImVec2(ImGui::GetCursorScreenPos()),
				ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - 16,
					ImGui::GetCursorScreenPos().y + ImGui::GetWindowHeight() - 60), col);

			ImGui::End();
		}
	}

	void EditorLayer::OnEvent(OverEngine::Event& event)
	{
	}

	void EditorLayer::MainDockSpace()
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

	void EditorLayer::MainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("(dummy menu)", NULL, false, false);
				if (ImGui::MenuItem("New")) {}
				if (ImGui::MenuItem("Open", "Ctrl+O")) {}
				if (ImGui::BeginMenu("Open Recent"))
				{
					ImGui::MenuItem("fish_hat.c");
					ImGui::MenuItem("fish_hat.inl");
					ImGui::MenuItem("fish_hat.h");
					if (ImGui::BeginMenu("More.."))
					{
						ImGui::MenuItem("Hello");
						ImGui::MenuItem("Sailor");

						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Save", "Ctrl+S")) {}
				if (ImGui::MenuItem("Save As..")) {}

				ImGui::Separator();
				if (ImGui::BeginMenu("Options"))
				{
					static bool enabled = true;
					ImGui::MenuItem("Enabled", "", &enabled);
					ImGui::BeginChild("child", ImVec2(0, 60), true);
					for (int i = 0; i < 10; i++)
						ImGui::Text("Scrolling Text %d", i);
					ImGui::EndChild();
					static float f = 0.5f;
					static int n = 0;
					ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
					ImGui::InputFloat("Input", &f, 0.1f);
					ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Colors"))
				{
					float sz = ImGui::GetTextLineHeight();
					for (int i = 0; i < ImGuiCol_COUNT; i++)
					{
						const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
						ImVec2 p = ImGui::GetCursorScreenPos();
						ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
						ImGui::Dummy(ImVec2(sz, sz));
						ImGui::SameLine();
						ImGui::MenuItem(name);
					}
					ImGui::EndMenu();
				}

				// Here we demonstrate appending again to the "Options" menu (which we already created above)
				// Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
				// In a real code-base using it would make senses to use this feature from very different code locations.
				if (ImGui::BeginMenu("Options")) // <-- Append!
				{
					static bool b = true;
					ImGui::Checkbox("SomeOption", &b);
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Disabled", false)) // Disabled
				{
					IM_ASSERT(0);
				}
				if (ImGui::MenuItem("Checked", NULL, true)) {}
				if (ImGui::MenuItem("Quit", "Alt+F4")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
}