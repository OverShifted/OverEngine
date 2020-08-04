#include "OnEditorGUI.h"

#include <imgui/imgui.h>

void OnMainMenubarGUI()
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

void OnMainDockSpaceGUI()
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

void OnToolbarGUI()
{
	static char a[32] = "";
	static char b[1024] = "";

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 8.0f, 4.0f });
	ImGui::Begin("Toolbar");

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 8.0f, 5.0f });

	if (ImGui::Button("New Project"))
	{
		ImGui::OpenPopup("New Project");
	}

	if (ImGui::BeginPopup("New Project"))
	{
		ImGui::InputText("Project Name", a, 32);
		ImGui::InputText("Project Directory", b, 1024);
		ImGui::Button("Create");
		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();

	ImGui::End();
	ImGui::PopStyleVar();
}

Vector2 OnViewportGUI(Ref<FrameBuffer> frameBuffer, Camera* camera)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

	ImGui::Begin("Viewport");

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)(intptr_t)frameBuffer->GetColorAttachmentRendererID(), viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	Vector2 viewportSize = Vector2(viewportPanelSize.x, viewportPanelSize.y);

	ImGui::End();

	ImGui::PopStyleVar();

	return viewportSize;
}

void OnSceneGraphGUI(Ref<Scene> scene)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

	ImGui::Begin("Scene Graph");

	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

	static int selection_mask = 0;
	int node_clicked = -1;

	for (uint32_t i = 0; i < scene->GetRootEntities().size(); i++)
	{
		auto& entity = scene->GetRootEntities()[i];

		ImGuiTreeNodeFlags node_flags = base_flags;
		const bool is_selected = (selection_mask & BIT(i)) != 0;
		if (is_selected)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, entity.GetComponent<NameComponent>().Name.c_str());
		if (ImGui::IsItemClicked())
			node_clicked = i;

		if (node_open)
		{
			ImGui::TreePop();
		}
	}

	if (node_clicked != -1)
	{
		// Update selection state
		// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
		if (ImGui::GetIO().KeyCtrl)
			selection_mask ^= BIT(node_clicked);          // CTRL+click to toggle
		else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
			selection_mask = BIT(node_clicked);           // Click to single-select
	}

	ImGui::End();

	ImGui::PopStyleVar();
}

void OnAssetsGUI()
{
	ImGui::Begin("Assets");

	ImGui::Text("Your Assets Here!");

	ImGui::End();
}

void OnConsoleGUI()
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