#include "pcheader.h"
#include "Application.h"

#include "OverEngine/Core/Log.h"
#include "OverEngine/Input/Input.h"
#include "OverEngine/Input/InputSystem.h"

#include "OverEngine/ImGui/ImGuiLayer.h"

#include "OverEngine/Renderer/Renderer.h"
#include "OverEngine/Renderer/Renderer2D.h"

namespace OverEngine
{
	Application* Application::s_Instance = nullptr;

	Application::Application(String name, bool useInternalRenderer)
		: m_UseInternalRenderer(useInternalRenderer)
	{
		s_Instance = this;

		WindowProps props = WindowProps(name, 1280, 720, true);
		m_Window = Window::Create(props);
		m_Window->SetEventCallback(OE_BIND_EVENT_FN(Application::OnEvent));

		if (m_UseInternalRenderer)
			Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		if (m_UseInternalRenderer)
			Renderer::Shutdown();
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
				layer->OnUpdate(Time::GetDeltaTime());

			if (m_ImGuiEnabled)
			{
				m_ImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
				m_ImGuiLayer->End();
			}
			
			GetMainWindow().OnUpdate();
			Time::RecalculateDeltaTime();
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

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}
}