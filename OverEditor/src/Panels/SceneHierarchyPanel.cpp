#include "SceneHierarchyPanel.h"

#include <OverEngine/Scene/Components.h>
#include "ComponentEditor.h"

#include <OverEngine/ImGui/ExtraImGui.h>
#include <imgui/imgui.h>

namespace OverEditor
{
	template<typename T>
	static void CheckComponentEditor(uint32_t componentTypeID, Entity entity)
	{
		if (componentTypeID == entt::type_info<T>::id())
		{
			ComponentEditor<T>(entity, componentTypeID);
			ImGui::Separator();
		}
	}

	template<typename T, typename... Args>
	static void CheckAddComponent(Entity& entity, const char* label, Args&&... args)
	{
		if (!entity.HasComponent<T>() && ImGui::Selectable(label))
			entity.AddComponent<T>(std::forward<Args>(args)...);
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<SceneEditor>& context)
		: m_Context(context)
	{
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		if (m_Context->Context)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
			ImGui::Begin("Scene Hierarchy");
			ImGui::PopStyleVar();

			Entity clickedEntity = RecursiveDraw();

			if (clickedEntity)
			{
				if (ImGui::GetIO().KeyCtrl)
				{
					auto it = std::find(m_Context->SelectionContext.begin(), m_Context->SelectionContext.end(), clickedEntity);
					if (it != m_Context->SelectionContext.end())
						m_Context->SelectionContext.erase(it);
					else
						m_Context->SelectionContext.push_back(clickedEntity);
				}
				else
				{
					m_Context->SelectionContext.clear();
					m_Context->SelectionContext.push_back(clickedEntity);
				}
			}

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 5.0f, 5.0f });
			if (ImGui::BeginPopupContextWindow("_SCENE_HIERARCHY_CONTEXTMENU"))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
				{
					Entity createdEntity = m_Context->Context->CreateEntity();
					m_Context->SelectionContext.clear();
					m_Context->SelectionContext.push_back(createdEntity);
				}

				ImGui::EndPopup();
			}
			ImGui::PopStyleVar();

			ImGui::BeginChild("Blank Space");
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
				m_Context->SelectionContext.clear();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 5.0f, 5.0f });
			if (ImGui::BeginPopupContextWindow("_SCENE_HIERARCHY_CONTEXTMENU"))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
				{
					Entity createdEntity = m_Context->Context->CreateEntity();
					m_Context->SelectionContext.clear();
					m_Context->SelectionContext.push_back(createdEntity);
				}

				ImGui::EndPopup();
			}
			ImGui::PopStyleVar();
			ImGui::EndChild();

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE_HIERARCHY_ENTITY_DRAG"))
				{
					Entity* otherEntity = static_cast<Entity*>(payload->Data);
					otherEntity->GetComponent<TransformComponent>().DetachFromParent();
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::End();
		}
		else
		{
			ImGui::Begin("Scene Hierarchy");

			const char* text = "Please open a scene to start editing!";

			ImVec2 textSize = ImGui::CalcTextSize(text);
			ImVec2 windowSize = ImGui::GetContentRegionAvail();
			ImGui::TextUnformatted(text);

			ImGui::End();
		}

		ImGui::Begin("Inspector");

		if (m_Context->SelectionContext.size() == 1)
		{
			auto& selectedEntity = m_Context->SelectionContext[0];

			char id[100];
			sprintf_s(id, sizeof(id) / sizeof(char), "INSPECTOR_ENTITY_EDITOR%i", (uint32_t)selectedEntity);
			ImGui::PushID(id);

			auto& base = selectedEntity.GetComponent<BaseComponent>();
			ImGui::InputText("##Name", &base.Name);

			bool wannaDestroy = false;
			if (ImGui::SmallButton("Destroy Entity"))
				wannaDestroy = true;

			ImGui::SameLine();

			ImGui::TextWrapped("GUID: %s", base.ID.ToString().c_str());

			ImGui::Separator();

			for (const auto& componentTypeID : selectedEntity.GetComponentsTypeIDList())
			{
				CheckComponentEditor<TransformComponent>(componentTypeID, selectedEntity);
				CheckComponentEditor<SpriteRendererComponent>(componentTypeID, selectedEntity);
				CheckComponentEditor<CameraComponent>(componentTypeID, selectedEntity);
			}

			if (ImGui::Button("Add Component##Button", ImVec2(-1.0f, 40.0f)))
				ImGui::OpenPopup("Add Component##Popup");

			if (ImGui::BeginPopup("Add Component##Popup"))
			{
				CheckAddComponent<TransformComponent>(selectedEntity, "Transform Component##AddComponentPopup");
				CheckAddComponent<SpriteRendererComponent>(selectedEntity, "SpriteRenderer Component##AddComponentPopup", nullptr);
				CheckAddComponent<CameraComponent>(selectedEntity, "Camera Component##AddComponentPopup");

				ImGui::EndPopup();
			}

			ImGui::PopID();

			if (wannaDestroy)
			{
				selectedEntity.Destroy();
				m_Context->SelectionContext.clear();
			}
		}
		else if (m_Context->SelectionContext.size() > 1)
		{
			ImGui::TextUnformatted("Cannot edit multiple entities at the same time!");
		}
		else
		{
			ImGui::TextUnformatted("Please select some thing to edit!");
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetContext(const Ref<SceneEditor>& context)
	{
		m_Context = context;
	}

	Entity SceneHierarchyPanel::RecursiveDraw(Entity parentEntity)
	{
		ImGuiTreeNodeFlags baseNodeFlags = OE_IMGUI_BASE_TREE_VIEW_FLAGS;

		Entity selectedEntity;

		Vector<entt::entity>* entityList;
		if (parentEntity)
			entityList = &(parentEntity.GetComponent<TransformComponent>().GetChildrenHandles());
		else
			entityList = &(m_Context->Context->GetRootHandles());

		for (auto entityHandle : *(entityList))
		{
			Entity entity{ entityHandle, m_Context->Context.get() };

			ImGuiTreeNodeFlags nodeFlags = baseNodeFlags;

			if (std::find(m_Context->SelectionContext.begin(), m_Context->SelectionContext.end(), entity) != m_Context->SelectionContext.end())
				nodeFlags |= ImGuiTreeNodeFlags_Selected;

			bool entityIsParent = entity.GetComponent<TransformComponent>().GetChildrenHandles().size();

			if (!entityIsParent)
				nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			const char* name = entity.GetComponent<BaseComponent>().Name.c_str();
			bool nodeIsOpen = ImGui::TreeNodeEx((void*)(intptr_t)(entity.GetRuntimeID()), nodeFlags, "%s", name);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10.0f, 10.0f });
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("_SCENE_HIERARCHY_ENTITY_DRAG", &entity, sizeof(Entity));
				ImGui::TextUnformatted(name);
				ImGui::EndDragDropSource();
			}
			ImGui::PopStyleVar();

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE_HIERARCHY_ENTITY_DRAG"))
				{
					Entity* otherEntity = static_cast<Entity*>(payload->Data);
					otherEntity->GetComponent<TransformComponent>().SetParent(entity);
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::IsItemClicked())
				selectedEntity = entity;

			if (entityIsParent && nodeIsOpen)
			{
				auto childClickedEntity = RecursiveDraw(entity);
				if (!selectedEntity)
					selectedEntity = childClickedEntity;

				ImGui::TreePop();
			}
		}

		return selectedEntity;
	}
}
