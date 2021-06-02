#pragma once

namespace OverEngine
{
	class ImGuiPanel
	{
	public:
		ImGuiPanel() = default;
		virtual ~ImGuiPanel() = default;

		virtual void OnImGuiRender() {}
		virtual void SetOpen(bool isOpen) {}
		virtual bool IsOpen() { return true; }
	};
}
