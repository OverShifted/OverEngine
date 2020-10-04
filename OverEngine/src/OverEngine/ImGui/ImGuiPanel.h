#pragma once

namespace OverEngine
{
	class ImGuiPanel
	{
	public:
		ImGuiPanel() = default;
		virtual ~ImGuiPanel() = default;

		virtual void OnImGuiRender() {}
		virtual void SetIsOpen(bool isOpen) {}
		virtual bool GetIsOpen() { return true; }
	};
}
