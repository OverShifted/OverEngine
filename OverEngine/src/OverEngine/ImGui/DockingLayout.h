#pragma once
#include <imgui.h>
#include <imgui_internal.h>

namespace OverEngine
{
	struct DockNode
	{
		DockNode() = default;

		ImGuiID ID;
		WeakRef<DockNode> ParentNode{};
		Ref<DockNode> ChildNodes[2] = { nullptr, nullptr }; // Child nodes (left/right or top/bottom).
		ImGuiDockNodeFlags Flags;

		Vector<String> Tabs;
		ImGuiID SelectedTab;        // Which of our tab/window is selected.

		Vector2 Position;
		Vector2 Size;

		float SplitRatio = 0.0f;    // Ratio of first child.
		ImGuiAxis SplitAxis;

		bool IsSplitNode()    const { return ChildNodes[0] != nullptr; }
		bool IsLeafNode()     const { return ChildNodes[0] == nullptr; }
	};

	class DockingLayout
	{
	public:
		DockingLayout() = default;

		// Get/Put data from/in ImGui Dockspace(s)
		void Apply(ImGuiID rootNode);
		void Read(ImGuiID rootNode);

		// Get/Put data from/in yaml file(s)
		void Load(const String& filepath);
		void Save(const String& filepath);

	private:
		Ref<DockNode> GetNode(const ImGuiID& id);

	private:
		Ref<DockNode> m_RootNode;
		Map<ImGuiID, Ref<DockNode>> m_Nodes;
	};
}
