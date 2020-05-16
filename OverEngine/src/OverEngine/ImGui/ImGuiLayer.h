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

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

		inline static float ClearColor[3] = { 0, 0, 0 };
	private:
		float m_Time = 0.0f;
		const char* Themes[5] = { "Dark", "Light", "Classic", "D1", "D2" };
		int c = 0;
	};
}