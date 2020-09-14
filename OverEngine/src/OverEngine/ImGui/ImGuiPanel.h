#pragma once

namespace OverEngine
{
	class ImGuiPanel
	{
	public:
		ImGuiPanel() = default;
		virtual ~ImGuiPanel() = default;

		virtual void OnImGuiRender() {}
	};
}
