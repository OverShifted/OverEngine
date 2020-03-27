#pragma once

#include "Core.h"
#include "OverEngine/Events/Event.h"
#include "OverEngine/Events/ApplicationEvent.h"

#include "Window.h"

namespace OverEngine {

	class OVER_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}