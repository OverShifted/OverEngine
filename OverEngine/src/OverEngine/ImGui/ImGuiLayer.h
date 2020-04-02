#pragma once

#include "OverEngine/Layers/Layer.h"

#include "OverEngine/Events/ApplicationEvent.h"
#include "OverEngine/Events/KeyEvent.h"
#include "OverEngine/Events/MouseEvent.h"

namespace OverEngine
{
	class OVER_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);

		inline static float ClearColor[3] = { 0, 0.15f , 0.09f };
	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);

	private:
		float m_Time = 0.0f;
		const char* Themes[3] = { "Dark", "Light", "Classic" };
		int c = 0;
	};
}