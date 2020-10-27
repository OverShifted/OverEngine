#include "AssetsPanel.h"
#include "Editor.h"

#include <OverEngine/Assets/Texture2DAsset.h>
#include <OverEngine/Assets/SceneAsset.h>

#include <OverEngine/ImGui/ExtraImGui.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <experimental/vector>

namespace OverEditor
{
	using namespace OverEngine;

	AssetsPanel::AssetsPanel(Editor* editor)
		: m_Editor(editor)
	{
	}

	void AssetsPanel::OnImGuiRender()
	{
		ImGui::Begin("Assets");

		if (!m_OneColumnView)
			ImGui::Columns(2);

		if (ImGui::Button("Settings"))
			ImGui::OpenPopup("SettingsPopup##AssetBrowser");

		if (ImGui::BeginPopup("SettingsPopup##AssetBrowser"))
		{
			if (ImGui::Checkbox("One Column View", &m_OneColumnView) && m_OneColumnView == false)
				std::experimental::erase_if(m_SelectionContext, [](const Ref<Asset> asset) {
					return !asset->IsFolder();
				});

			ImGui::EndPopup();
		}

		const auto& rootAsset = m_Editor->GetProject()->GetAssets().GetAsset("/");

		ImGuiTreeNodeFlags rootNodeFlags = OE_IMGUI_BASE_TREE_VIEW_FLAGS;
		if (std::find(m_SelectionContext.begin(), m_SelectionContext.end(), rootAsset) != m_SelectionContext.end())
			rootNodeFlags |= ImGuiTreeNodeFlags_Selected;

		bool rootOpen = ImGui::TreeNodeEx("##AssetsRootTreeNode", rootNodeFlags, "/");
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
				auto it = std::find(m_SelectionContext.begin(), m_SelectionContext.end(), clickedItem);
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
					count += (uint32_t)(TYPE_PAWN(a, Ref<FolderAsset>)->GetAssets().size());
				else
					count++;
			}

			ImVec2 thumbnailSize((float)m_ThumbnailSize, (float)m_ThumbnailSize);
			ImGuiStyle& style = ImGui::GetStyle();
			float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
			static const ImVec2 threeDotsTextSize = ImGui::CalcTextSize("...");

			uint32_t n = 0;
			for (const auto& a : m_SelectionContext)
			{
				if (!a->IsFolder())
					continue;

				for (const auto& asset : TYPE_PAWN(a, Ref<FolderAsset>)->GetAssets())
				{
					ImGui::PushID(n);
					if (asset->GetType() == AssetType::Texture2D)
					{
						ImGui::ImageButton(TYPE_PAWN(asset, Ref<Texture2DAsset>)->GetTextures()[0].second, thumbnailSize);

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::TextUnformatted(asset->GetName().c_str());
							ImGui::EndTooltip();
						}

						ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10.0f, 10.0f });
						if (ImGui::BeginDragDropSource())
						{
							ImGui::SetDragDropPayload("_TEXTURE2D_DRAG", &(TYPE_PAWN(asset, Ref<Texture2DAsset>)->GetTextures()[0].second), sizeof(Ref<Texture2D>));
							ImGui::TextUnformatted(asset->GetName().c_str());
							ImGui::EndDragDropSource();
						}
						ImGui::PopStyleVar();
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.2f, 0.2f, 0.2f });
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.2f, 0.2f, 0.5f });
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.2f, 0.2f, 0.7f });
						if (asset->IsFolder())
							ImGui::ImageButton(m_Editor->GetIcons()["FolderIcon"], thumbnailSize, -1, { 0.0f, 0.0f, 0.0f, 0.0f });
						else if (asset->GetType() == AssetType::Scene)
							ImGui::ImageButton(m_Editor->GetIcons()["SceneIcon"], thumbnailSize, -1, { 0.0f, 0.0f, 0.0f, 0.0f });
						else
							ImGui::Button("", thumbnailSize);
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
								m_Editor->EditScene(TYPE_PAWN(asset, Ref<SceneAsset>)->GetScene(), asset->GetPath());
							}
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::TextUnformatted(asset->GetName().c_str());
							ImGui::EndTooltip();
						}
					}

					ImVec2 rectMin = ImGui::GetItemRectMin();
					ImVec2 textSize = ImGui::CalcTextSize(asset->GetName().c_str());
					if (textSize.x + 10 <= thumbnailSize.x) // Enough space
					{
						ImVec2 textPos = { rectMin.x + (thumbnailSize.x - textSize.x) / 2, rectMin.y + thumbnailSize.y + 15 };
						windowDrawList->AddRectFilled({ textPos.x - 5, textPos.y }, { textPos.x + textSize.x + 5, textPos.y + textSize.y }, ImColor(20, 20, 20), 5);
						windowDrawList->AddText(textPos, ImColor(1.0f, 1.0f, 1.0f), asset->GetName().c_str());
					}
					else // Tight
					{
						ImVec2 posMax = { rectMin.x + thumbnailSize.x - 5, rectMin.y + thumbnailSize.y + 30 };
						windowDrawList->AddRectFilled({ rectMin.x, rectMin.y + thumbnailSize.y + 15 }, { posMax.x + 5, posMax.y }, ImColor(20, 20, 20), 5);
						ImGui::RenderTextEllipsis(windowDrawList, { rectMin.x + 5, rectMin.y + thumbnailSize.y + 15 }, posMax, posMax.x, posMax.x, asset->GetName().c_str(), nullptr, &textSize);
					}

					float last_button_x2 = ImGui::GetItemRectMax().x;
					float next_button_x2 = last_button_x2 + style.ItemSpacing.x + thumbnailSize.x;
					if (n + 1 < count && next_button_x2 < window_visible_x2)
						ImGui::SameLine();
					else
						ImGui::Dummy({ 0, 25 });

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

		for (const auto& asset : TYPE_PAWN(assetToDraw, Ref<FolderAsset>)->GetAssets())
		{
			if (!m_OneColumnView && !asset->IsFolder())
				continue;

			ImGuiTreeNodeFlags node_flags = OE_IMGUI_BASE_TREE_VIEW_FLAGS;
			if (std::find(m_SelectionContext.begin(), m_SelectionContext.end(), asset) != m_SelectionContext.end())
				node_flags |= ImGuiTreeNodeFlags_Selected;

			if (!asset->IsFolder())
				node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			bool nodeIsOpen = ImGui::TreeNodeEx(asset->GetGuid().ToString().c_str(), node_flags, "%s", asset->GetName().c_str());

			// EXTREAMLY TEMPO
			if (asset->GetType() == AssetType::Texture2D)
			{
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(asset->GetName().c_str());
					ImGui::Image(TYPE_PAWN(asset, Ref<Texture2DAsset>)->GetTextures()[0].second, { 128, 128 });
					ImGui::EndTooltip();
				}

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10.0f, 10.0f });
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("_TEXTURE2D_DRAG", &(TYPE_PAWN(asset, Ref<Texture2DAsset>)->GetTextures()[0]), sizeof(Ref<Texture2D>));
					ImGui::TextUnformatted(asset->GetName().c_str());
					ImGui::Image(TYPE_PAWN(asset, Ref<Texture2DAsset>)->GetTextures()[0].second, { 128, 128 });
					ImGui::EndDragDropSource();
				}
				ImGui::PopStyleVar();
			}

			if (ImGui::IsItemClicked())
			{
				if (!asset->IsFolder() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (asset->GetType() == AssetType::Scene)
						m_Editor->EditScene(TYPE_PAWN(asset, Ref<SceneAsset>)->GetScene(), asset->GetPath());
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
}
