#pragma once
#include <imgui.h>
#include <imgui_internal.h>

namespace OverEngine
{
	struct GuiWindowData
	{
		String Name;        // Full window name might include `##`
		ImGuiID ID;         // Window ID
		bool Active;        // Is window active (focused)?

		bool Docked;        // Is window docked some where?
		ImGuiID DockNodeID; // Where the window is docked?

		Vector2 Position;   // Position of window in pixels relative to top left corner of screen
		Vector2 Size;       // Size of window in pixels
	};

	struct DockNode
	{
		DockNode() = default;

		ImGuiID ID;
		Ref<DockNode> ParentNode = nullptr;
		Ref<DockNode> ChildNodes[2] = { nullptr, nullptr }; // Child nodes (left/right or top/bottom).
		ImGuiDockNodeFlags Flags;

		Vector<String> Tabs;
		ImGuiID SelectedTab;        // Which of our tab/window is selected.

		Vector2 Position;
		Vector2 Size;

		float SplitRatio = 0.0f;    // Ratio of first child.
		ImGuiAxis SplitAxis;

		bool IsRootNode()     const { return ParentNode == nullptr; }
		bool IsDockSpace()    const { return Flags & ImGuiDockNodeFlags_DockSpace; }
		bool IsFloatingNode() const { return IsRootNode() && !IsDockSpace(); }
		bool IsCentralNode()  const { return Flags & ImGuiDockNodeFlags_CentralNode; }
		bool IsHiddenTabBar() const { return Flags & ImGuiDockNodeFlags_HiddenTabBar; }
		bool HasTabBar()      const { return Flags & ImGuiDockNodeFlags_NoTabBar; }
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
