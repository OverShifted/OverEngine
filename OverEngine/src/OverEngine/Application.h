#pragma once

#include "Core.h"

#include "Window.h"
#include "OverEngine/Layers/LayerStack.h"
#include "OverEngine/Events/Event.h"
#include "OverEngine/Events/ApplicationEvent.h"

namespace OverEngine {

	class OVER_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		inline static Application& Get() { return *m_Instance; }
		inline Window& GetMainWindow() { return *m_Windows[m_MainWindow]; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Windows[1];
		int m_MainWindow = 0;
		bool m_Running = true;
		LayerStack m_LayerStack;

		static Application* m_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}