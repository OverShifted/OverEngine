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

#include "OverEngine/Input/InputSystem.h"

namespace OverEngine {

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
		inline Window& GetMainWindow() { return *m_Windows[m_MainWindow]; }

		void InputSystemTestCallBack(InputAction::TriggerInfo& triggerInfo);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Vector<Scope<Window>> m_Windows;
		int m_MainWindow;

		bool m_Running = true;
		bool m_Minimized = false;

		LayerStack        m_LayerStack;
		ImGuiLayer*       m_ImGuiLayer;

		Ref<Shader> m_Shader;
		Ref<VertexArray> m_VertexArray;

		Ref<Shader> m_BlueShader;
		Ref<VertexArray> m_SquareVA;

		Ref<InputActionMap> m_InputMap;
		Ref<InputAction>    m_InputAction;
		Ref<InputBinding>   m_InputBinding;

	private:
		static Application* m_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}