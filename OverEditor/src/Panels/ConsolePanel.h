#pragma once

#include <OverEngine.h>
#include <OverEngine/ImGui/ImGuiPanel.h>

namespace OverEditor
{
	using namespace OverEngine;

	class ConsolePanel : public ImGuiPanel
	{
	public:
		enum class Level : int8_t
		{
			Trace    =  0,
			Debug    =  1,
			Info     =  2,
			Warn     =  3,
			Error    =  4,
			Critical =  5,
			Off      =  6
		};

		struct Message
		{
			String LoggerName;
			String MessageString;

			Level MessageLevel;
		};
	public:
		ConsolePanel() = default;

		virtual void OnImGuiRender() override;

		virtual void SetIsOpen(bool isOpen) override { m_IsOpen = isOpen; }
		virtual bool GetIsOpen() override { return m_IsOpen; }

		void AddMessage(const Ref<Message>& message);
	private:
		bool m_IsOpen;

		bool m_ShowInfo    = true;
		bool m_ShowWarning = true;
		bool m_ShowError   = true;

		Vector<Ref<Message>> m_Messages;
	};
}
