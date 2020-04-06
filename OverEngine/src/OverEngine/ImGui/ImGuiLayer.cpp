#include "pcheader.h"
#include "ImGuiLayer.h"

#include "imgui.h"

#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "OverEngine/Application.h"
#include "OverEngine/Input/Input.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace OverEngine
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcon;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

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
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
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
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show = false;
		ImGui::ShowDemoWindow(&show);

		ImGuiIO& io = ImGui::GetIO();

		ImFont* Font = io.Fonts->AddFontFromFileTTF("d:/d.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesDefault());
		OE_INFO(io.Fonts->Locked);

		//ImGui::PushFont(Font);
		//OE_INFO("{0}, {1}", Font->IsLoaded());
		ImGui::Begin("Prefrences");
		if (ImGui::CollapsingHeader("User Interface"))
		{
			if (ImGui::Combo("Theme", &c, Themes, 3))
			{
				switch (c)
				{
				case 0:
					ImGui::StyleColorsDark();
					break;
				case 1:
					ImGui::StyleColorsLight();
					break;
				case 2:
					ImGui::StyleColorsClassic();
					break;
				default:
					break;
				}
			}
		}

		if (ImGui::CollapsingHeader("Clear Color"))
		{
			ImGui::ColorEdit3("Clear Color", ClearColor);
		}
		ImGui::End();

		ImGui::Begin("TestX");
		ImGui::Text("Hell World!");
		ImGui::Text("I am rendering from OvereNfine!");
		ImGui::End();

		//ImGui::PopFont();
	}
	/*
	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetMainWindow().GetWidth(), (float)app.GetMainWindow().GetHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
		m_Time = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Begin("Prefrences");
		if (ImGui::CollapsingHeader("User Interface"))
		{
			if (ImGui::Combo("Theme", &c, Themes, 3))
			{
				switch (c)
				{
				case 0:
					ImGui::StyleColorsDark();
					break;
				case 1:
					ImGui::StyleColorsLight();
					break;
				case 2:
					ImGui::StyleColorsClassic();
					break;
				default:
					break;
				}
			}
		}

		if (ImGui::CollapsingHeader("Clear Color"))
		{
			ImGui::ColorEdit3("Clear Color", ClearColor);
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}*/
}