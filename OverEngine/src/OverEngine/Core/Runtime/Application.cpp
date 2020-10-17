#include "pcheader.h"
#include "Application.h"

#include "OverEngine/Core/Log.h"
#include "OverEngine/Core/Random.h"
#include "OverEngine/Input/InputSystem.h"

#include "OverEngine/ImGui/ImGuiLayer.h"

#include "OverEngine/Renderer/Renderer.h"

namespace OverEngine
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationProps& props)
	{
		s_Instance = this;

		// Init features
		Runtime::Init(props.RuntimeType);
		Log::Init();
		Random::Init();

		#ifdef _MSC_VER
		OE_CORE_INFO("OverEngine v0.0 [MSC {}]", _MSC_VER);
		#else
		OE_CORE_INFO("OverEngine v0.0");
		#endif

		// Some window should exist to init Renderer
		m_Window = Window::Create(props.MainWindowProps);
		m_Window->SetEventCallback(BIND_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
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
		dispatcher.Dispatch<WindowCloseEvent> (BIND_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_FN(Application::OnWindowResize));

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
			
			m_Window->OnUpdate();
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
