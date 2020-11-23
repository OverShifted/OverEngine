#include "SceneHierarchyPanel.h"

#include <OverEngine/Scene/Components.h>
#include "ComponentEditor.h"

#include <OverEngine/ImGui/ExtraImGui.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

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
					auto it = STD_CONTAINER_FIND(m_Context->SelectionContext, clickedEntity);
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

			char buffer[35];
			sprintf_s(buffer, OE_ARRAY_SIZE(buffer), "INSPECTOR_ENTITY_EDITOR%i", (uint32_t)selectedEntity);
			ImGui::PushID(buffer);

			sprintf_s(buffer, OE_ARRAY_SIZE(buffer), "0x%llx", selectedEntity.GetComponent<IDComponent>().ID);

			ImGui::InputText(buffer, &selectedEntity.GetComponent<NameComponent>().Name);

			bool wannaDestroy = false;
			if (ImGui::Button("Destroy Entity"))
				wannaDestroy = true;

			ImGui::Separator();

			for (const auto& componentTypeID : selectedEntity.GetComponentsTypeIDList())
			{
				CheckComponentEditor<TransformComponent>(componentTypeID, selectedEntity);
				CheckComponentEditor<SpriteRendererComponent>(componentTypeID, selectedEntity);
				CheckComponentEditor<CameraComponent>(componentTypeID, selectedEntity);
				CheckComponentEditor<RigidBody2DComponent>(componentTypeID, selectedEntity);
				CheckComponentEditor<Colliders2DComponent>(componentTypeID, selectedEntity);
			}

			if (ImGui::Button("Add Component##Button", ImVec2(-1.0f, 40.0f)))
				ImGui::OpenPopup("Add Component##Popup");

			if (ImGui::BeginPopup("Add Component##Popup"))
			{
				CheckAddComponent<TransformComponent>(selectedEntity, "Transform Component##AddComponentPopup");
				CheckAddComponent<SpriteRendererComponent>(selectedEntity, "SpriteRenderer Component##AddComponentPopup", nullptr);
				CheckAddComponent<CameraComponent>(selectedEntity, "Camera Component##AddComponentPopup");
				CheckAddComponent<RigidBody2DComponent>(selectedEntity, "RigidBody2D Component##AddComponentPopup");
				CheckAddComponent<Colliders2DComponent>(selectedEntity, "Colliders2D Component##AddComponentPopup");

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
		Entity selectedEntity;

		Scene* activeScene = m_Context->GetActiveScene();
		Vector<entt::entity>* entityList;

		if (parentEntity)
		{
			entityList = &parentEntity.GetComponent<TransformComponent>().GetChildrenHandles();
		}
		else
		{
			entityList = &activeScene->GetRootHandles();
		}

		for (auto entityHandle : *(entityList))
		{
			Entity entity{ entityHandle, activeScene };

			ImGuiTreeNodeFlags nodeFlags = OE_IMGUI_BASE_TREE_VIEW_FLAGS | ImGuiTreeNodeFlags_AllowItemOverlap;

			if (STD_CONTAINER_FIND(m_Context->SelectionContext, entity) != m_Context->SelectionContext.end())
				nodeFlags |= ImGuiTreeNodeFlags_Selected;

			auto& tc = entity.GetComponent<TransformComponent>();
			bool entityIsParent = tc.GetChildrenHandles().size();

			if (!entityIsParent)
				nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			const char* name = entity.GetComponent<NameComponent>().Name.c_str();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
			bool nodeIsOpen = ImGui::TreeNodeBehavior(ImGui::GetCurrentWindow()->GetID((void*)(intptr_t)(entity.GetRuntimeID())), nodeFlags, name);
			ImGui::PopStyleVar();

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

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
			ImGui::InvisibleButton("_ENTITY_ORDER_SETER", { -1, 5 });
			ImGui::PopStyleVar();

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE_HIERARCHY_ENTITY_DRAG"))
				{
					auto& other = *static_cast<Entity*>(payload->Data);
					if (entity != other)
					{
						auto& otherTc = static_cast<Entity*>(payload->Data)->GetComponent<TransformComponent>();

						if (entityIsParent && nodeIsOpen)
						{
							otherTc.SetParent(entity);
							otherTc.SetSiblingIndex(0);
						}
						else if (entity != tc.GetParent())
						{
							otherTc.SetParent(tc.GetParent());
							otherTc.SetSiblingIndex(tc.GetSiblingIndex() + 1);
						}
					}
				}
				ImGui::EndDragDropTarget();
			}

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
