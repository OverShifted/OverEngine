#include "pcheader.h"
#include "Application.h"

#include "OverEngine/Core/Log.h"
#include "OverEngine/Core/Random.h"

#include "OverEngine/ImGui/ImGuiLayer.h"

#include "OverEngine/Renderer/Renderer.h"

namespace OverEngine
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationProps& props)
	{
		s_Instance = this;

		// Init features
		Log::Init();
		Random::Init();

		#ifdef OE_RELEASE
			const char* buildType = "Release";
		#elif defined(OE_DEBUG)
			const char* buildType = "Debug";
		#else
			const char* buildType = "Unknown build type";
		#endif

		#ifdef __clang__
			OE_CORE_INFO("OverEngine v0.0 [CLANG LLVM {}.{}.{}] [{}]", __clang_major__, __clang_minor__, __clang_patchlevel__, buildType);
		#elif defined(__GNUC__)
			OE_CORE_INFO("OverEngine v0.0 [GCC {}.{}.{}] [{}]", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__, buildType);
		#elif defined(_MSC_VER)
			OE_CORE_INFO("OverEngine v0.0 [MSC {}] [{}]", _MSC_VER, buildType);
		#else
			OE_CORE_INFO("OverEngine v0.0 [UNKNOWN COMPILER] [{}]", buildType);
		#endif

		// To initialize the Renderer a Window should exist.
		// Because Context creating is happened when a Window in created.
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
