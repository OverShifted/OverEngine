#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Window.h"
#include "OverEngine/Core/Time/Time.h"

#include "OverEngine/ImGui/ImGuiLayer.h"

#include "OverEngine/Layers/LayerStack.h"
#include "OverEngine/Events/Event.h"
#include "OverEngine/Events/ApplicationEvent.h"

#include "OverEngine/Renderer/Shader.h"
#include "OverEngine/Renderer/Buffer.h"
#include "OverEngine/Renderer/VertexArray.h"

namespace OverEngine
{

	class OVER_API Application
	{
	public:
		Application(String name = "OverEngine");
		virtual ~Application() = default;

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		inline static Application& Get() { return *m_Instance; }
		inline Window& GetMainWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> m_Window;

		bool m_Running      = true;
		bool m_Minimized    = false;
		bool m_ImGuiEnabled = false;

		LayerStack        m_LayerStack;
		ImGuiLayer*       m_ImGuiLayer;
	private:
		static Application* m_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}