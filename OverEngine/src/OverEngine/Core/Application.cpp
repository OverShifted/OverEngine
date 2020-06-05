#include "pcheader.h"
#include "Application.h"

#include "OverEngine/Core/Log.h"
#include "OverEngine/Input/Input.h"
#include "OverEngine/Input/InputSystem.h"

#include "OverEngine/ImGui/ImGuiLayer.h"

#include "OverEngine/Renderer/Renderer.h"

namespace OverEngine
{
	Application* Application::m_Instance = nullptr;

	Application::Application(String name) 
	{
		m_Instance = this;

		WindowProps props = WindowProps(name, 1280, 720, true);
		m_Windows.push_back(Scope<Window>(Window::Create(props)));
		m_MainWindow = 0;

		m_Windows[m_MainWindow]->SetEventCallback(OE_BIND_EVENT_FN(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent> (OE_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(OE_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			(*it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		// Game Loop
		while (m_Running)
		{	
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();
			
			GetMainWindow().OnUpdate(); // Poll Events Here // Events Here
			InputSystem::OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
	
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		GetMainWindow().GetRendererContext()->SetViewport(e.GetWidth(), e.GetHeight());

		return false;
	}
}