#include "ConsolePanel.h"
#include <imgui/imgui.h>

namespace OverEditor
{
	void ConsolePanel::OnImGuiRender()
	{
		ImGui::Begin("Console");

		if (ImGui::Button("Clear"))
			m_Messages.clear();

		ImGui::SameLine();

		ImGui::Checkbox("Info", &m_ShowInfo);
		ImGui::SameLine();
		ImGui::Checkbox("Warning", &m_ShowWarning);
		ImGui::SameLine();
		ImGui::Checkbox("Error", &m_ShowError);

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f });
		ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		{
			for (auto message : m_Messages)
			{
				if ((message->MessageLevel == Level::Trace || message->MessageLevel == Level::Info) && !m_ShowInfo
					|| message->MessageLevel == Level::Warn && !m_ShowWarning
					|| (message->MessageLevel == Level::Error || message->MessageLevel == Level::Critical) && !m_ShowError)
					continue;

				ImVec4 color;
				switch (message->MessageLevel)
				{
				case Level::Trace   : color = { 1.0f, 1.0f, 1.0f, 1.0f }; break;
				case Level::Info    : color = { 0.0f, 1.0f, 0.0f, 1.0f }; break;
				case Level::Warn    : color = { 1.0f, 1.0f, 0.0f, 1.0f }; break;
				case Level::Error   : color = { 1.0f, 0.0f, 0.0f, 1.0f }; break;
				case Level::Critical: color = { 1.0f, 0.0f, 0.0f, 1.0f }; break;
				}
				ImGui::TextColored(color, "%s", message->MessageString.c_str());
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::End();
	}

	void ConsolePanel::AddMessage(const Ref<Message>& message)
	{
		m_Messages.push_back(message);
	}
}
