#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Runtime/Runtime.h"
#include "OverEngine/Core/Window.h"
#include "OverEngine/Core/Time/Time.h"

#include "OverEngine/Layers/LayerStack.h"
#include "OverEngine/Events/Event.h"
#include "OverEngine/Events/ApplicationEvent.h"

namespace OverEngine
{
	struct ApplicationProps
	{
		WindowProps MainWindowProps;
		OverEngine::RuntimeType RuntimeType = RuntimeType::Player;
	};

	class ImGuiLayer;
	class Application
	{
	public:
		Application(const ApplicationProps& props = ApplicationProps());
		virtual ~Application();

		void Run();
		inline void Close() { m_Running = false; }

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_Instance; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		inline Window& GetWindow() { return *m_Window; }
	protected:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> m_Window;

		bool m_Running   = true;
		bool m_Minimized = false;

		LayerStack  m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;
		bool m_ImGuiEnabled = false;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication(int argc, char** argv);
}
