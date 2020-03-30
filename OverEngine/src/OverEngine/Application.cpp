#include "pcheader.h"
#include "Application.h"

#include "OverEngine/Log.h"

#include <glad/glad.h>

#include "OverEngine/DataStructs/Vector2.h"

namespace OverEngine {
	Application* Application::m_Instance = nullptr;

	Application::Application() 
	{
		m_Instance = this;
		m_Windows[0] = std::unique_ptr<Window>(Window::Create());
		m_Windows[0]->SetEventCallback(OE_BIND_EVENT_FN(Application::OnEvent));
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(OE_BIND_EVENT_FN(Application::OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
		OE_CORE_TRACE(e);
	}

	void Application::Run()
	{

		Vector2 v = Vector2(5, 10);
		OE_INFO(v);

		while (m_Running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			OE_CORE_CRITICAL("Hello");
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_Windows[m_MainWindow]->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

}