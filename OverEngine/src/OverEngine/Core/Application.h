#pragma once

#include "Core.h"

#include "Window.h"

#include "OverEngine/ImGui/ImGuiLayer.h"

#include "OverEngine/Layers/LayerStack.h"
#include "OverEngine/Events/Event.h"
#include "OverEngine/Events/ApplicationEvent.h"

#include "OverEngine/Renderer/Shader.h"
#include "OverEngine/Renderer/Buffer.h"
#include "OverEngine/Renderer/VertexArray.h"

namespace OverEngine {

	class OVER_API Application
	{
	public:
		Application(std::string name = "OverEngine");
		virtual ~Application() = default;

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		inline static Application& Get() { return *m_Instance; }
		inline Window& GetMainWindow() { return *m_Windows[m_MainWindow]; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		std::vector<std::unique_ptr<Window>> m_Windows;
		int m_MainWindow;

		bool m_Running = true;
		bool m_Minimized = false;

		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<Shader> m_BlueShader;
		std::shared_ptr<VertexArray> m_SquareVA;
	private:
		static Application* m_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}