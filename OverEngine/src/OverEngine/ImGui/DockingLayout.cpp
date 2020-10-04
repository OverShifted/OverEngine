#include "pcheader.h"
#include "DockingLayout.h"

namespace OverEngine
{
	void DockingLayout::Init(ImGuiID rootNode)
	{
		static bool init = false;

		if (init)
			return;
		init = true;

		ImGui::DockBuilderRemoveNode(rootNode); // Clear out existing layout
		ImGui::DockBuilderAddNode(rootNode, ImGuiDockNodeFlags_DockSpace); // Add empty node
		ImGui::DockBuilderSetNodeSize(rootNode, ImGui::GetMainViewport()->Size);

		ImGuiID dock_main_id = rootNode;
		ImGuiID dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.50f, NULL, &dock_main_id);
		ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.80f, NULL, &dock_main_id);

		ImGui::DockBuilderDockWindow("Toolbar", dock_id_bottom);
		ImGui::DockBuilderDockWindow("Project Manager", dock_id_prop);
		ImGui::DockBuilderFinish(rootNode);
	}
}
