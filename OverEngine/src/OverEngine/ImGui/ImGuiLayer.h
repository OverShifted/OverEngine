#pragma once

#include "OverEngine/Layers/Layer.h"

#include "OverEngine/Events/ApplicationEvent.h"
#include "OverEngine/Events/KeyEvent.h"
#include "OverEngine/Events/MouseEvent.h"

namespace OverEngine
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void SetStyle();

		void SetBlockEvents(bool block) { m_BlockEvents = block; }
		bool GetBlockEvents() { return m_BlockEvents; }

	private:
		bool m_BlockEvents = true;
	};
}
