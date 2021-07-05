#include "AssetsPanel.h"
#include "EditorLayer.h"

#include <OverEngine/Renderer/Texture.h>
#include <OverEngine/Scene/Scene.h>

#include <OverEngine/ImGui/ExtraImGui.h>
#include <UI/UIElements.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace OverEditor
{
	using namespace OverEngine;

	AssetsPanel::AssetsPanel()
	{
	}

	void AssetsPanel::OnImGuiRender()
	{
		ImGui::Begin("Assets");

		if (m_SelectionContext)
		{
			ImGui::Begin("Asset Settings");
			ImGui::TextUnformatted(m_SelectionContext->GetName().c_str());
			ImGui::TextUnformatted(static_cast<String>(m_SelectionContext->GetPath()).c_str());
			ImGui::End();
		}

		if (!m_OneColumnView)
			ImGui::Columns(2);

		if (ImGui::Button("Settings"))
			ImGui::OpenPopup("SettingsPopup##AssetBrowser");

		if (ImGui::BeginPopup("SettingsPopup##AssetBrowser"))
		{
			if (ImGui::Checkbox("One Column View", &m_OneColumnView) && m_OneColumnView == false)
			{
				if (m_SelectionContext->GetClassName() != AssetFolder::GetStaticClassName())
					m_SelectionContext = nullptr;
			}

			ImGui::EndPopup();
		}

		const auto& rootAsset = AssetDatabase::GetRoot();

		ImGuiTreeNodeFlags rootNodeFlags = OE_IMGUI_BASE_TREE_VIEW_FLAGS;

		if (m_SelectionContext == rootAsset)
			rootNodeFlags |= ImGuiTreeNodeFlags_Selected;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
		bool rootOpen = ImGui::TreeNodeEx("##AssetsRootTreeNode", rootNodeFlags, "/");
		ImGui::PopStyleVar();

		Ref<Asset> clickedItem = nullptr;

		if (ImGui::IsItemClicked())
			clickedItem = rootAsset;

		if (rootOpen)
		{
			auto childClickedItem = RecursiveDraw(rootAsset);
			if (!clickedItem)
				clickedItem = childClickedItem;

			ImGui::TreePop();
		}

		if (clickedItem)
		{
			m_SelectionContext = clickedItem;
		}

		//////////////////////////////////////////////////////
		// Thumbnails ////////////////////////////////////////
		//////////////////////////////////////////////////////

		static bool firstCall = true;

		if (!m_OneColumnView)
		{
			if (firstCall)
			{
				ImGui::SetColumnWidth(0, 250.0f);
				firstCall = false;
			}

			ImGui::NextColumn();

			ImGui::PushItemWidth(-1);
			ImGui::Spacing();
			{
				int tSize = m_ThumbnailSize;
				if (ImGui::SliderInt("##Thumbnail Size", &tSize, m_ThumbnailSizeMin, m_ThumbnailSizeMax, "Thumbnail Size : %dpx"))
					m_ThumbnailSize = tSize;
			}
			ImGui::PopItemWidth();

			if (m_SelectionContext)
			{
				if (ImGui::ArrowButton("AssetThumbnailUpFolderButton", ImGuiDir_Up))
				{
					String parentPath = m_SelectionContext->GetPath();
					parentPath = parentPath.substr(0, parentPath.find_last_of('/'));
					if (parentPath.empty())
						parentPath = "/";

					if (auto parent = AssetDatabase::GetAssetByPath(parentPath))
						m_SelectionContext = parent;
				}

				ImGui::SameLine();
				ImGui::TextUnformatted(m_SelectionContext->GetPath().c_str());
			}

			ImGui::BeginChild("##ThumbnailView", { 0, 0 }, false, ImGuiWindowFlags_AlwaysUseWindowPadding);

			Ref<AssetFolder> selectionFolder = std::dynamic_pointer_cast<AssetFolder>(m_SelectionContext);

			uint32_t count = 0;
			if (selectionFolder)
				count = (uint32_t)selectionFolder->GetAssets().size();

			uint32_t n = 0;
			if (selectionFolder)
			{
				for (const auto& asset : selectionFolder->GetAssets())
				{
					ImGui::PushID(n);
					DrawThumbnail(asset.second, n + 1 == count);
					ImGui::PopID();
					n++;
				}
			}
			ImGui::EndChild();
		}

		if (!m_OneColumnView)
			ImGui::Columns(1);

		ImGui::End();
	}

	Ref<Asset> AssetsPanel::RecursiveDraw(const Ref<AssetFolder>& assetToDraw)
	{
		Ref<Asset> selectedItem;

		for (const auto& nameAndAsset : assetToDraw->GetAssets())
		{
			const auto& asset = nameAndAsset.second;

			if (!m_OneColumnView && asset->GetClassName() != AssetFolder::GetStaticClassName())
				continue;

			ImGuiTreeNodeFlags nodeFlags = OE_IMGUI_BASE_TREE_VIEW_FLAGS;

			if (m_SelectionContext == asset)
				nodeFlags |= ImGuiTreeNodeFlags_Selected;

			if (asset->GetClassName() != AssetFolder::GetStaticClassName())
				nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
			bool nodeIsOpen = ImGui::TreeNodeEx((void*)asset.get(), nodeFlags, asset->GetName().c_str());
			ImGui::PopStyleVar();

			if (asset->GetClassName() == Texture2D::GetStaticClassName())
			{
				UIElements::Tooltip([&asset]() {
					ImGui::TextUnformatted(asset->GetName().c_str());
					ImGui::Image(std::dynamic_pointer_cast<Texture2D>(asset), { 128, 128 });
				});

				UIElements::Texture2DDragSource(std::dynamic_pointer_cast<Texture2D>(asset), asset->GetName().c_str(), true);
			}

			if (ImGui::IsItemClicked())
			{
				if (asset->GetClassName() != AssetFolder::GetStaticClassName() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (asset->GetClassName() == Scene::GetStaticClassName())
						EditorLayer::Get().EditScene(std::dynamic_pointer_cast<Scene>(asset));
				}

				selectedItem = asset;
			}

			if (asset->GetClassName() == AssetFolder::GetStaticClassName() && nodeIsOpen)
			{
				if (!selectedItem)
					selectedItem = RecursiveDraw(std::dynamic_pointer_cast<AssetFolder>(asset));

				ImGui::TreePop();
			}
		}

		return selectedItem;
	}

	void AssetsPanel::DrawThumbnail(Ref<Asset> asset, bool last)
	{
		ImVec2 thumbnailSize{ (float)m_ThumbnailSize, (float)m_ThumbnailSize };

		Ref<Texture2D> icon = nullptr;

		if (asset->GetClassName() == AssetFolder::GetStaticClassName())
		{
			icon = EditorLayer::Get().GetIcons()["FolderIcon"];
		}
		else if (asset->GetClassName() == Texture2D::GetStaticClassName())
		{
			icon = std::dynamic_pointer_cast<Texture2D>(asset);
		}
		else if (asset->GetClassName() == Scene::GetStaticClassName())
		{
			icon = EditorLayer::Get().GetIcons()["SceneIcon"];
		}

		ImGui::PushStyleColor(ImGuiCol_Button,        { 0.2f, 0.2f, 0.2f, 0.2f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.2f, 0.2f, 0.5f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  { 0.2f, 0.2f, 0.2f, 0.7f });

		if (icon)
		{
			ImGui::ImageButton(icon, thumbnailSize);
		}
		else
		{
			ImGui::Button("", thumbnailSize);
		}

		ImGui::PopStyleColor(3);

		if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (asset->GetClassName() == AssetFolder::GetStaticClassName())
			{
				m_SelectionContext = asset;
			}
			else if (asset->GetClassName() == Scene::GetStaticClassName())
			{
				EditorLayer::Get().EditScene(std::dynamic_pointer_cast<Scene>(asset));
			}
		}

		UIElements::Tooltip([&asset]() {
			ImGui::TextUnformatted(asset->GetName().c_str());
		});

		if (asset->GetClassName() == Texture2D::GetStaticClassName())
		{
			UIElements::Texture2DDragSource(std::dynamic_pointer_cast<Texture2D>(asset), asset->GetName().c_str());
		}
		
		//////////////////////////////////////////////////////
		// Text //////////////////////////////////////////////
		//////////////////////////////////////////////////////

		ImDrawList& windowDrawList = *ImGui::GetWindowDrawList();

		ImVec2 rectMin = ImGui::GetItemRectMin();
		ImVec2 rectSize = ImGui::GetItemRectSize();
		ImVec2 textSize = ImGui::CalcTextSize(asset->GetName().c_str());

		static Vector2 padding{ 5, 5 };
		static int marginTop = 15;

		if (textSize.x + padding.x * 2 <= rectSize.x) // Enough space
		{
			/*

			<----------> = rectSize.x

			rectMin
			||
			\/
			*___________
			|    O     | /\
			|  --|--   | ||	                             // Closer look to the text
			|    |     | || = rectSize.y                 __________
			|   / \    | ||                              |        | => padding.y
			|__________| \/     \/                       | Player |
			                    || = marginTop           |________| => padding.y
			  |Player|          /\                       ||      ||
			                                             \/      \/
			  <------> = textSize.x                  padding.x   padding.x
			| |
			<-> = (rectSize.x - textSize.x) / 2

			*/

			float rectMin_x = rectMin.x - padding.x + (rectSize.x - textSize.x) / 2;
			float rectMin_y = rectMin.y + rectSize.y + marginTop;

			float rectMax_x = rectMin_x + textSize.x + padding.x * 2;
			float rectMax_y = rectMin_y + textSize.y + padding.y * 2;

			windowDrawList.AddRectFilled({ rectMin_x, rectMin_y }, { rectMax_x, rectMax_y }, ImColor(20, 20, 20), 5);
			windowDrawList.AddText({ rectMin_x + padding.x, rectMin_y + padding.y }, ImColor(1.0f, 1.0f, 1.0f), asset->GetName().c_str());
		}
		else // Tight => use ellipsis
		{
			float rectMin_y = rectMin.y + rectSize.y + marginTop;

			float rectMax_x = rectMin.x + rectSize.x;
			float rectMax_y = rectMin_y + textSize.y + padding.y * 2;

			windowDrawList.AddRectFilled({ rectMin.x, rectMin_y }, { rectMax_x, rectMax_y }, ImColor(20, 20, 20), 5);

			rectMax_x -= padding.x;
			rectMax_y -= padding.y;

			ImGui::RenderTextEllipsis(&windowDrawList, { rectMin.x + padding.x, rectMin_y + padding.y }, { rectMax_x, rectMax_y }, rectMax_x, rectMax_x, asset->GetName().c_str(), nullptr, &textSize);
		}

		//////////////////////////////////////////////////////
		// Wrapping //////////////////////////////////////////
		//////////////////////////////////////////////////////

		float nextBtn = ImGui::GetItemRectMax().x + ImGui::GetStyle().ItemSpacing.x + rectSize.x;
		float rightMost = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

		if (!last && nextBtn < rightMost)
			ImGui::SameLine();
		else
			ImGui::Dummy({ 0, 50 });
	}
}
