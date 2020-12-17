#include "AssetsPanel.h"
#include "EditorLayer.h"

#include <OverEngine/Assets/Texture2DAsset.h>
#include <OverEngine/Assets/SceneAsset.h>

#include <OverEngine/ImGui/ExtraImGui.h>
#include <UI/UIElements.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <experimental/vector>

namespace OverEditor
{
	using namespace OverEngine;

	AssetsPanel::AssetsPanel()
	{
	}

	void AssetsPanel::OnImGuiRender()
	{
		ImGui::Begin("Assets");

		if (m_SelectionContext.size() == 1)
		{
			auto asset = m_SelectionContext[0];
			ImGui::Begin("Asset Settings");
			ImGui::TextUnformatted(asset->GetName().c_str());
			ImGui::TextUnformatted(asset->GetPath().c_str());
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
				std::experimental::erase_if(m_SelectionContext, [](const Ref<Asset>& asset) {
					return !asset->IsFolder();
				});
			}

			ImGui::EndPopup();
		}

		const auto& rootAsset = EditorLayer::Get().GetProject()->GetAssets().GetAsset("/");

		ImGuiTreeNodeFlags rootNodeFlags = OE_IMGUI_BASE_TREE_VIEW_FLAGS;

		if (STD_CONTAINER_HAS(m_SelectionContext, rootAsset))
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
			if (ImGui::GetIO().KeyCtrl)
			{
				auto it = STD_CONTAINER_FIND(m_SelectionContext, clickedItem);

				if (it != m_SelectionContext.end())
					m_SelectionContext.erase(it);
				else
					m_SelectionContext.push_back(clickedItem);
			}
			else
			{
				m_SelectionContext.clear();
				m_SelectionContext.push_back(clickedItem);
			}
		}

		//////////////////////////////////////////////////////
		// Thumbnails ////////////////////////////////////////
		//////////////////////////////////////////////////////

		if (!m_OneColumnView)
		{
			ImGui::NextColumn();

			ImGui::PushItemWidth(-1);
			ImGui::Spacing();
			{
				int tSize = m_ThumbnailSize;
				if (ImGui::SliderInt("##Thumbnail Size", &tSize, m_ThumbnailSizeMin, m_ThumbnailSizeMax, "Thumbnail Size : %dpx"))
					m_ThumbnailSize = tSize;
			}
			ImGui::PopItemWidth();

			if (m_SelectionContext.size() > 1)
				ImGui::TextUnformatted("Showing multiple files and folders");
			else if (m_SelectionContext.size() == 1)
			{
				if (ImGui::ArrowButton("AssetThumbnailBackButton", ImGuiDir_Left))
				{
					auto parent = m_SelectionContext[0]->GetParent();
					if (parent)
						m_SelectionContext[0] = m_SelectionContext[0]->GetParent();
				}
				ImGui::SameLine();
				ImGui::TextUnformatted(m_SelectionContext[0]->GetName().c_str());
			}

			ImGui::BeginChild("##ThumbnailView", { 0, 0 }, false, ImGuiWindowFlags_AlwaysUseWindowPadding);

			uint32_t count = 0;
			for (const auto& a : m_SelectionContext)
			{
				if (a->IsFolder())
					count += (uint32_t)a->GetFolderAsset()->GetAssets().size();
			}

			uint32_t n = 0;
			for (const auto& a : m_SelectionContext)
			{
				if (!a->IsFolder())
					continue;

				for (const auto& asset : a->GetFolderAsset()->GetAssets())
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

	Ref<Asset> AssetsPanel::RecursiveDraw(const Ref<Asset>& assetToDraw)
	{
		Ref<Asset> selectedItem;

		for (const auto& nameAndAsset : assetToDraw->GetFolderAsset()->GetAssets())
		{
			const auto& asset = nameAndAsset.second;

			if (!m_OneColumnView && !asset->IsFolder())
				continue;

			ImGuiTreeNodeFlags nodeFlags = OE_IMGUI_BASE_TREE_VIEW_FLAGS;

			if (STD_CONTAINER_HAS(m_SelectionContext, asset))
				nodeFlags |= ImGuiTreeNodeFlags_Selected;

			if (!asset->IsFolder())
				nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
			bool nodeIsOpen = ImGui::TreeNodeBehavior(ImGui::GetCurrentWindow()->GetID((void*)asset->GetGuid()), nodeFlags, asset->GetName().c_str());
			ImGui::PopStyleVar();

			// EXTREAMLY TEMPO
			if (asset->GetType() == AssetType::Texture2D)
			{
				UIElements::Tooltip([&asset]() {
					ImGui::TextUnformatted(asset->GetName().c_str());
					ImGui::Image(asset->GetTexture2DAsset()->GetTextures().begin()->second, { 128, 128 });
				});

				UIElements::Texture2DDragSource(asset->GetTexture2DAsset()->GetTextures().begin()->second, asset->GetName().c_str(), true);
			}

			if (ImGui::IsItemClicked())
			{
				if (!asset->IsFolder() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (asset->GetType() == AssetType::Scene)
						EditorLayer::Get().EditScene(TYPE_PAWN(asset, Ref<SceneAsset>));
				}

				selectedItem = asset;
			}

			if (asset->IsFolder() && nodeIsOpen)
			{
				if (!selectedItem)
					selectedItem = RecursiveDraw(asset);

				ImGui::TreePop();
			}
		}

		return selectedItem;
	}

	void AssetsPanel::DrawThumbnail(const Ref<Asset>& asset, bool last)
	{
		ImVec2 thumbnailSize{ (float)m_ThumbnailSize, (float)m_ThumbnailSize };

		Ref<Texture2D> icon = nullptr;

		if (asset->IsFolder())
		{
			icon = EditorLayer::Get().GetIcons()["FolderIcon"];
		}
		else if (asset->GetType() == AssetType::Texture2D)
		{
			// TODO: Show all textures
			icon = asset->GetTexture2DAsset()->GetTextures().begin()->second;
		}
		else if (asset->GetType() == AssetType::Scene)
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
			if (asset->IsFolder())
			{
				m_SelectionContext.clear();
				m_SelectionContext.push_back(asset);
			}
			else if (asset->GetType() == AssetType::Scene)
			{
				EditorLayer::Get().EditScene(TYPE_PAWN(asset, Ref<SceneAsset>));
			}
		}

		UIElements::Tooltip([&asset]() {
			ImGui::TextUnformatted(asset->GetName().c_str());
		});

		if (asset->GetType() == AssetType::Texture2D)
		{
			UIElements::Texture2DDragSource(asset->GetTexture2DAsset()->GetTextures().begin()->second, asset->GetName().c_str());
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
