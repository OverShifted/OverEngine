#pragma once
#include <imgui.h>
#include <imgui_internal.h>

namespace OverEngine
{
	struct DockingLayoutNode
	{
		DockingLayoutNode* ParentNode;
		DockingLayoutNode* ChildNodes[2];

		Direction SplitDirection = Direction::None;
		float SplitRatio;
	};

	class DockingLayout
	{
	public:
		DockingLayout() = default;
		void Init(ImGuiID rootNode);
	private:
		DockingLayoutNode m_RootNode;
	};
}
