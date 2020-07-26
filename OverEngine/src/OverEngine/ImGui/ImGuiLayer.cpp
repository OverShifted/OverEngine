#include "pcheader.h"
#include "ImGuiLayer.h"

#include "imgui/imgui.h"

#include "fonts/Roboto.h"

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "imgui/examples/imgui_impl_opengl3.h"
#include "imgui/examples/imgui_impl_glfw.h"

#include "OverEngine/Core/Application.h"
#include "OverEngine/Input/Input.h"

namespace OverEngine
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		//io.ConfigViewportsNoTaskBarIcon = true;
		//io.ConfigViewportsNoAutoMerge = true;
		io.ConfigViewportsNoDecoration = false;

		// Setup Dear ImGui style
		ApplyTheme(ImguiThemes::DarkVariant3);

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetMainWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");

		//io.Fonts->AddFontFromFileTTF("Roboto-Light.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesDefault());


		io.Fonts->AddFontFromMemoryCompressedTTF(&Roboto_compressed_data, Roboto_compressed_size, 15.0f);


		//style.WindowRounding = 5.3f;
		//style.GrabRounding = style.FrameRounding = 2.3f;
		//style.ScrollbarRounding = 5.0f;
		//style.FrameBorderSize = 1.0f;
		//style.ItemSpacing.y = 6.5f;
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender()
	{
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetMainWindow().GetWidth(), (float)app.GetMainWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			Application::Get().GetMainWindow().GetRendererContext()->Current();
		}
	}

	void ImGuiLayer::ApplyTheme(ImguiThemes theme)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		switch (theme)
		{
		case ImguiThemes::Classic:
			ImGui::StyleColorsClassic();
			break;
		case ImguiThemes::Dark:
			ImGui::StyleColorsDark();
			break;
		case ImguiThemes::Light:
			ImGui::StyleColorsLight();
			break;
		case ImguiThemes::DarkVariant1:
			style.Colors[ImGuiCol_Text]                 = { 0.73333335f, 0.73333335f, 0.73333335f, 1.00f };
			style.Colors[ImGuiCol_TextDisabled]         = { 0.34509805f, 0.34509805f, 0.34509805f, 1.00f };
			style.Colors[ImGuiCol_WindowBg]             = { 0.23529413f, 0.24705884f, 0.25490198f, 0.94f };
			style.Colors[ImGuiCol_ChildBg]              = { 0.23529413f, 0.24705884f, 0.25490198f, 0.00f };
			style.Colors[ImGuiCol_PopupBg]              = { 0.23529413f, 0.24705884f, 0.25490198f, 0.94f };
			style.Colors[ImGuiCol_Border]               = { 0.33333334f, 0.33333334f, 0.33333334f, 0.50f };
			style.Colors[ImGuiCol_BorderShadow]         = { 0.15686275f, 0.15686275f, 0.15686275f, 0.00f };
			style.Colors[ImGuiCol_FrameBg]              = { 0.16862746f, 0.16862746f, 0.16862746f, 0.54f };
			style.Colors[ImGuiCol_FrameBgHovered]       = { 0.453125f, 0.67578125f, 0.99609375f, 0.67f };
			style.Colors[ImGuiCol_FrameBgActive]        = { 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };
			style.Colors[ImGuiCol_TitleBg]              = { 0.04f, 0.04f, 0.04f, 1.00f };
			style.Colors[ImGuiCol_TitleBgCollapsed]     = { 0.16f, 0.29f, 0.48f, 1.00f };
			style.Colors[ImGuiCol_TitleBgActive]        = { 0.00f, 0.00f, 0.00f, 0.51f };
			style.Colors[ImGuiCol_MenuBarBg]            = { 0.27058825f, 0.28627452f, 0.2901961f, 0.80f };
			style.Colors[ImGuiCol_ScrollbarBg]          = { 0.27058825f, 0.28627452f, 0.2901961f, 0.60f };
			style.Colors[ImGuiCol_ScrollbarGrab]        = { 0.21960786f, 0.30980393f, 0.41960788f, 0.51f };
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
			style.Colors[ImGuiCol_ScrollbarGrabActive]  = { 0.13725491f, 0.19215688f, 0.2627451f, 0.91f };
			style.Colors[ImGuiCol_CheckMark]            = { 0.90f, 0.90f, 0.90f, 0.83f };
			style.Colors[ImGuiCol_SliderGrab]           = { 0.70f, 0.70f, 0.70f, 0.62f };
			style.Colors[ImGuiCol_SliderGrabActive]     = { 0.30f, 0.30f, 0.30f, 0.84f };
			style.Colors[ImGuiCol_Button]               = { 0.33333334f, 0.3529412f, 0.36078432f, 0.49f };
			style.Colors[ImGuiCol_ButtonHovered]        = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
			style.Colors[ImGuiCol_ButtonActive]         = { 0.13725491f, 0.19215688f, 0.2627451f, 1.00f };
			style.Colors[ImGuiCol_Header]               = { 0.33333334f, 0.3529412f, 0.36078432f, 0.53f };
			style.Colors[ImGuiCol_HeaderHovered]        = { 0.453125f, 0.67578125f, 0.99609375f, 0.67f };
			style.Colors[ImGuiCol_HeaderActive]         = { 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };
			style.Colors[ImGuiCol_Separator]            = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
			style.Colors[ImGuiCol_SeparatorHovered]     = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
			style.Colors[ImGuiCol_SeparatorActive]      = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
			style.Colors[ImGuiCol_ResizeGrip]           = { 1.00f, 1.00f, 1.00f, 0.85f };
			style.Colors[ImGuiCol_ResizeGripHovered]    = { 1.00f, 1.00f, 1.00f, 0.60f };
			style.Colors[ImGuiCol_ResizeGripActive]     = { 1.00f, 1.00f, 1.00f, 0.90f };
			style.Colors[ImGuiCol_PlotLines]            = { 0.61f, 0.61f, 0.61f, 1.00f };
			style.Colors[ImGuiCol_PlotLinesHovered]     = { 1.00f, 0.43f, 0.35f, 1.00f };
			style.Colors[ImGuiCol_PlotHistogram]        = { 0.90f, 0.70f, 0.00f, 1.00f };
			style.Colors[ImGuiCol_PlotHistogramHovered] = { 1.00f, 0.60f, 0.00f, 1.00f };
			style.Colors[ImGuiCol_TextSelectedBg]       = { 0.18431373f, 0.39607847f, 0.79215693f, 0.90f };
			break;
		case ImguiThemes::DarkVariant2:
			style.Colors[ImGuiCol_Text]                  = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
			style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
			style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
			style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
			style.Colors[ImGuiCol_Border]                = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
			style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
			style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
			style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
			style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
			style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
			style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_Button]                = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_Header]                = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
			style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_Separator]             = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
			style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
			style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
			style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
			style.Colors[ImGuiCol_Tab]                   = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
			style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			style.Colors[ImGuiCol_TabActive]             = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
			style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
			style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
			style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
			style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
			style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
			break;
		case ImguiThemes::DarkVariant3:
			style.Colors[ImGuiCol_Text]                  = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
			style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
			style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
			style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
			style.Colors[ImGuiCol_Border]                = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
			style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
			style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
			style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
			style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
			style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
			style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_Button]                = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_Header]                = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
			style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_Separator]             = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
			style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
			style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
			style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
			style.Colors[ImGuiCol_Tab]                   = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
			style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			style.Colors[ImGuiCol_TabActive]             = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
			style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
			style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
			style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
			style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
			style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
			style.FramePadding                           = ImVec2(8.0f, 6.0f);
			style.ItemSpacing                            = ImVec2(8.0f, 8.0f);
			style.WindowMenuButtonPosition               = ImGuiDir_Right;
			style.TabRounding                            = 0.0f;
			break;
		}
	}
}