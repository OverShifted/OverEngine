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
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

		enum class ImguiThemes {
			Dark, Light, Classic, // BuiltIn
			DarkVariant1, DarkVariant2, DarkVariant3
		};

		void ApplyTheme(ImguiThemes theme);
	private:
		float m_Time = 0.0f;
	};
}