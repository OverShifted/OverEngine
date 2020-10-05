#include "AssetsPanel.h"
#include "Editor.h"

#include <OverEngine/Assets/Resource.h>
#include <OverEngine/ImGui/ExtraImGui.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

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
				for (auto it = m_SelectionContext.begin(); it < m_SelectionContext.end(); it++)
					if (!(*it)->IsDirectory())
						it = m_SelectionContext.erase(it);

			ImGui::EndPopup();
		}

		const auto& rootResource = m_Editor->GetProject()->GetResources().GetResource("assets://");

		ImGuiTreeNodeFlags rootNodeFlags = OE_IMGUI_BASE_TREE_VIEW_FLAGS;
		if (std::find(m_SelectionContext.begin(), m_SelectionContext.end(), rootResource) != m_SelectionContext.end())
			rootNodeFlags |= ImGuiTreeNodeFlags_Selected;

		bool rootOpen = ImGui::TreeNodeEx("##AssetsRootTreeNode", rootNodeFlags, "assets://");
		Ref<Resource> clickedItem = nullptr;

		if (ImGui::IsItemClicked())
			clickedItem = rootResource;

		if (rootOpen)
		{
			auto childClickedItem = RecursiveDraw(rootResource);
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
			ImGui::SliderInt("##Thumbnail Size", &m_ThumbnailSize, m_ThumbnailSizeMin, m_ThumbnailSizeMax, "Thumbnail Size : %d px");
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
			for (const auto& res : m_SelectionContext)
			{
				if (res->IsDirectory())
					count += (uint32_t)res->GetChildren().size();
				else
					count++;
			}

			ImVec2 thumbnailSize((float)m_ThumbnailSize, (float)m_ThumbnailSize);
			ImGuiStyle& style = ImGui::GetStyle();
			float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
			static const ImVec2 threeDotsTextSize = ImGui::CalcTextSize("...");

			uint32_t n = 0;
			for (const auto& res : m_SelectionContext)
			{
				if (!res->IsDirectory())
					continue;

				for (const auto& resource : res->GetChildren())
				{
					ImGui::PushID(n);
					if (resource->GetType() == ResourceType::Texture2D)
					{
						ImGui::ImageButton((*(Ref<Texture2DAsset>*)&resource->GetAssets()[0])->GetAsset(), thumbnailSize);

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::TextUnformatted(resource->GetName().c_str());
							ImGui::EndTooltip();
						}

						ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10.0f, 10.0f });
						if (ImGui::BeginDragDropSource())
						{
							ImGui::SetDragDropPayload("_ASSET_DRAG", &resource->GetAssets()[0], sizeof(Ref<Asset>));
							ImGui::TextUnformatted(resource->GetName().c_str());
							ImGui::EndDragDropSource();
						}
						ImGui::PopStyleVar();
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.2f, 0.2f, 0.2f });
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.2f, 0.2f, 0.5f });
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.2f, 0.2f, 0.7f });
						if (resource->GetType() == ResourceType::Directory)
							ImGui::ImageButton(m_Editor->GetIcons()["FolderIcon"], thumbnailSize, -1, { 0.0f, 0.0f, 0.0f, 0.0f });
						else if (resource->GetType() == ResourceType::Scene)
							ImGui::ImageButton(m_Editor->GetIcons()["SceneIcon"], thumbnailSize, -1, { 0.0f, 0.0f, 0.0f, 0.0f });
						else
							ImGui::Button("", thumbnailSize);
						ImGui::PopStyleColor(3);

						if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							if (resource->IsDirectory())
							{
								m_SelectionContext.clear();
								m_SelectionContext.push_back(resource);
							}
							else if (resource->GetType() == ResourceType::Scene && resource->GetAssets()[0]->GetType() == AssetType::Scene)
							{
								m_Editor->EditScene((*(Ref<SceneAsset>*)(&resource->GetAssets()[0]))->GetAsset(), resource->GetVirtualPath());
							}
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::TextUnformatted(resource->GetName().c_str());
							ImGui::EndTooltip();
						}
					}

					ImVec2 rectMin = ImGui::GetItemRectMin();
					ImVec2 textSize = ImGui::CalcTextSize(resource->GetName().c_str());
					if (textSize.x + 10 <= thumbnailSize.x) // Enough space
					{
						ImVec2 textPos = { rectMin.x + (thumbnailSize.x - textSize.x) / 2, rectMin.y + thumbnailSize.y + 15 };
						windowDrawList->AddRectFilled({ textPos.x - 5, textPos.y }, { textPos.x + textSize.x + 5, textPos.y + textSize.y }, ImColor(20, 20, 20), 5);
						windowDrawList->AddText(textPos, ImColor(1.0f, 1.0f, 1.0f), resource->GetName().c_str());
					}
					else // Tight
					{
						ImVec2 posMax = { rectMin.x + thumbnailSize.x - 5, rectMin.y + thumbnailSize.y + 30 };
						windowDrawList->AddRectFilled({ rectMin.x, rectMin.y + thumbnailSize.y + 15 }, { posMax.x + 5, posMax.y }, ImColor(20, 20, 20), 5);
						ImGui::RenderTextEllipsis(windowDrawList, { rectMin.x + 5, rectMin.y + thumbnailSize.y + 15 }, posMax, posMax.x, posMax.x, resource->GetName().c_str(), nullptr, &textSize);
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

	Ref<Resource> AssetsPanel::RecursiveDraw(const Ref<Resource>& resourceToDraw)
	{
		Ref<Resource> selectedItem;

		for (const auto& resource : resourceToDraw->GetChildren())
		{
			if (!m_OneColumnView && !resource->IsDirectory())
				continue;

			ImGuiTreeNodeFlags node_flags = OE_IMGUI_BASE_TREE_VIEW_FLAGS;
			if (std::find(m_SelectionContext.begin(), m_SelectionContext.end(), resource) != m_SelectionContext.end())
				node_flags |= ImGuiTreeNodeFlags_Selected;

			if (!resource->IsDirectory())
				node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			bool nodeIsOpen = ImGui::TreeNodeEx(resource->GetGuid().ToString().c_str(), node_flags, "%s", resource->GetName().c_str());

			// EXTREAMLY TEMPO
			if (resource->GetType() == ResourceType::Texture2D)
			{
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(resource->GetName().c_str());
					ImGui::Image((*(Ref<Texture2DAsset>*) & resource->GetAssets()[0])->GetAsset(), { 128, 128 });
					ImGui::EndTooltip();
				}

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10.0f, 10.0f });
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("_ASSET_DRAG", &resource->GetAssets()[0], sizeof(Ref<Asset>));
					ImGui::TextUnformatted(resource->GetName().c_str());
					ImGui::Image((*(Ref<Texture2DAsset>*) & resource->GetAssets()[0])->GetAsset(), { 128, 128 });
					ImGui::EndDragDropSource();
				}
				ImGui::PopStyleVar();
			}

			if (ImGui::IsItemClicked())
			{
				if (!resource->IsDirectory() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (resource->GetType() == ResourceType::Scene && resource->GetAssets()[0]->GetType() == AssetType::Scene)
						m_Editor->EditScene((*(Ref<SceneAsset>*)(&resource->GetAssets()[0]))->GetAsset(), resource->GetVirtualPath());
				}

				selectedItem = resource;
			}

			if (resource->IsDirectory() && nodeIsOpen)
			{
				if (!selectedItem)
					selectedItem = RecursiveDraw(resource);

				ImGui::TreePop();
			}
		}

		return selectedItem;
	}

}
