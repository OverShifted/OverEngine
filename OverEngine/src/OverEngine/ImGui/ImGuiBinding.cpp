#include "pcheader.h"
#include "ImGuiBinding.h"

#include "OverEngine/Renderer/RendererAPI.h"
#include "OverEngine/Core/Window.h"

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "imgui/examples/imgui_impl_opengl3.h"
#include "imgui/examples/imgui_impl_glfw.h"

struct GLFWwindow;

namespace OverEngine
{
	void(*ImGuiBinding::s_RendererShutdownFunction)();
	void(*ImGuiBinding::s_PlatformShutdownFunction)();

	void(*ImGuiBinding::s_RendererNewFrameFunction)();
	void(*ImGuiBinding::s_PlatformNewFrameFunction)();

	void(*ImGuiBinding::s_RenderFunction)(ImDrawData*);

	void ImGuiBinding::Init(Window* window)
	{
	#if (defined OE_PLATFORM_WINDOWS) || (defined OE_PLATFORM_LINUX)
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			break;
		case RendererAPI::API::OpenGL:
			ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window->GetNativeWindow()), true);
			break;
		}

		s_PlatformShutdownFunction = ImGui_ImplGlfw_Shutdown;
		s_PlatformNewFrameFunction = ImGui_ImplGlfw_NewFrame;
	#endif

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			break;
		case RendererAPI::API::OpenGL:
			ImGui_ImplOpenGL3_Init("#version 410");
			s_RendererShutdownFunction = ImGui_ImplOpenGL3_Shutdown;
			s_RendererNewFrameFunction = ImGui_ImplOpenGL3_NewFrame;
			s_RenderFunction = ImGui_ImplOpenGL3_RenderDrawData;
			break;
		}
	}
}