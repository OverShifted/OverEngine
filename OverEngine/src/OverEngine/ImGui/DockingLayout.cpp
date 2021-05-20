#include "pcheader.h"
#include "DockingLayout.h"

#include "OverEngine/Core/Runtime/Serialization/YamlConverters.h"
#include <yaml-cpp/yaml.h>

#include <fstream>

namespace OverEngine
{
	static void ApplyRecursive(const Ref<DockNode>& node, ImGuiID nodeID)
	{
		auto imNode = ImGui::DockBuilderGetNode(nodeID);

		imNode->LocalFlags = node->Flags;
		imNode->Pos = { node->Position.x, node->Position.y };
		imNode->Size = { node->Size.x, node->Size.y };

		ImGui::DockBuilderSetNodePos(nodeID, imNode->Pos);
		ImGui::DockBuilderSetNodeSize(nodeID, imNode->Size);

		if (node->IsSplitNode())
		{
			ImGuiID child_0, child_1;
			ImGuiDir direction = node->SplitAxis == ImGuiAxis_X ? ImGuiDir_Left : ImGuiDir_Up;
			ImGui::DockBuilderSplitNode(nodeID, direction, node->SplitRatio, &child_0, &child_1);
			ApplyRecursive(node->ChildNodes[0], child_0);
			ApplyRecursive(node->ChildNodes[1], child_1);
		}
		else if (node->IsLeafNode())
		{
			for (const auto& name : node->Tabs)
				ImGui::DockBuilderDockWindow(name.c_str(), nodeID);

			imNode->SelectedTabId = node->SelectedTab;
		}
	}

	void DockingLayout::Apply(ImGuiID rootNode)
	{
		ImGui::DockBuilderRemoveNode(rootNode); // Clear out existing layout
		ImGui::DockBuilderAddNode(rootNode, ImGuiDockNodeFlags_DockSpace); // Add empty node
		ImGui::DockBuilderSetNodeSize(rootNode, ImGui::GetMainViewport()->Size);

		ApplyRecursive(m_RootNode, rootNode);

		ImGui::DockBuilderFinish(rootNode);
	}

	static void ReadNodeRecursive(ImGuiID nodeID, Ref<DockNode> node, Map<ImGuiID, Ref<DockNode>>& nodes)
	{
		ImGuiDockNode* uiNode = ImGui::DockBuilderGetNode(nodeID);

		if (!uiNode)
			return;

		node->ID = uiNode->ID;

		if (uiNode->IsSplitNode())
		{
			node->ChildNodes[0] = CreateRef<DockNode>();
			node->ChildNodes[0]->ParentNode = node;
			nodes[uiNode->ChildNodes[0]->ID] = node->ChildNodes[0];
			ReadNodeRecursive(uiNode->ChildNodes[0]->ID, node->ChildNodes[0], nodes);

			node->ChildNodes[1] = CreateRef<DockNode>();
			node->ChildNodes[1]->ParentNode = node;
			nodes[uiNode->ChildNodes[1]->ID] = node->ChildNodes[1];
			ReadNodeRecursive(uiNode->ChildNodes[1]->ID, node->ChildNodes[1], nodes);
		}

		node->Flags = uiNode->GetMergedFlags();

		if (uiNode->TabBar)
		{
			for (const auto& tab : uiNode->TabBar->Tabs)
				node->Tabs.push_back(tab.Window->Name);

			node->SelectedTab = uiNode->SelectedTabId;
		}

		node->Position = IMVEC2_2_VECTOR2(uiNode->Pos);
		node->Size = IMVEC2_2_VECTOR2(uiNode->Size);

		if (uiNode->IsSplitNode())
		{
			node->SplitAxis = uiNode->SplitAxis;
			node->SplitRatio = uiNode->ChildNodes[0]->Size[uiNode->SplitAxis] / uiNode->SizeRef[uiNode->SplitAxis];
		}
	}

	void DockingLayout::Read(ImGuiID rootNode)
	{
		m_Nodes.clear();
		m_RootNode = CreateRef<DockNode>();
		ReadNodeRecursive(rootNode, m_RootNode, m_Nodes);
	}

	void DockingLayout::Load(const String& filepath)
	{
		YAML::Node data = YAML::LoadFile(filepath);

		m_RootNode = nullptr;
		m_Nodes.clear();

		// Node Object => Parent ID
		Map<Ref<DockNode>, ImGuiID> parentAssignList;

		for (auto dockNodeProps : data)
		{
			auto node = CreateRef<DockNode>();

			ImGuiID id = dockNodeProps["ID"].as<ImGuiID>();
			node->ID = id;

			if (!m_RootNode)
			{
				m_RootNode = node;
			}
			else
			{
				m_Nodes[id] = node;

				// Can be nullptr
				ImGuiID parentID = dockNodeProps["Parent"].as<ImGuiID>();
				auto parent = GetNode(parentID);

				if (parent)
				{
					node->ParentNode = parent;

					if (parent->ChildNodes[0])
						parent->ChildNodes[1] = node;
					else
						parent->ChildNodes[0] = node;
				}
				else
				{
					parentAssignList[node] = parentID;
				}
			}

			node->Flags = dockNodeProps["Flags"].as<ImGuiDockNodeFlags>();

			if (dockNodeProps["Tabs"])
			{
				node->Tabs = dockNodeProps["Tabs"].as<Vector<String>>();
				node->SelectedTab = dockNodeProps["SelectedTab"].as<ImGuiID>();
			}

			node->Position = dockNodeProps["Position"].as<Vector2>();
			node->Size = dockNodeProps["Size"].as<Vector2>();

			if (dockNodeProps["SplitRatio"])
			{
				node->SplitRatio = dockNodeProps["SplitRatio"].as<float>();
				node->SplitAxis = (ImGuiAxis)dockNodeProps["SplitAxis"].as<int>();
			}
		}

		for (auto& parentAssignCmd : parentAssignList)
		{
			const auto& node = parentAssignCmd.first;
			const auto& parent = GetNode(parentAssignCmd.second);

			node->ParentNode = parent;

			if (parent->ChildNodes[0])
				parent->ChildNodes[1] = node;
			else
				parent->ChildNodes[0] = node;
		}
	}

	static void SerializeNode(const Ref<DockNode>& node, YAML::Emitter& out)
	{
		out << YAML::BeginMap;

		out << YAML::Key << "ID" << YAML::Value << YAML::Hex << node->ID;

		if (node->ParentNode)
			out << YAML::Key << "Parent" << YAML::Value << YAML::Hex << node->ParentNode->ID;
		else
			out << YAML::Key << "Parent" << YAML::Value << YAML::Null;

		out << YAML::Key << "Flags" << YAML::Value << YAML::Dec << node->Flags;

		if (!node->Tabs.empty())
		{
			out << YAML::Key << "Tabs" << YAML::Value << YAML::Flow << node->Tabs;
			out << YAML::Key << "SelectedTab" << YAML::Value << YAML::Hex << node->SelectedTab;
		}

		out << YAML::Key << "Position" << YAML::Value << node->Position;
		out << YAML::Key << "Size" << YAML::Value << node->Size;

		if (node->IsSplitNode())
		{
			out << YAML::Key << "SplitRatio" << YAML::Value << node->SplitRatio;
			out << YAML::Key << "SplitAxis" << YAML::Value << (int)node->SplitAxis;
		}

		out << YAML::EndMap;
	}

	void DockingLayout::Save(const String& filepath)
	{
		YAML::Emitter out;

		out << YAML::BeginSeq;

		SerializeNode(m_RootNode, out);
		for (const auto& node : m_Nodes)
			SerializeNode(node.second, out);

		out << YAML::EndSeq;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	Ref<DockNode> DockingLayout::GetNode(const ImGuiID& id)
	{
		if (id == m_RootNode->ID)
			return m_RootNode;
		return m_Nodes[id];
	}
}
