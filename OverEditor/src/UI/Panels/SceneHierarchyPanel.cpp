#include "SceneHierarchyPanel.h"

#include <OverEngine/Scene/Components.h>
#include "UI/ComponentEditor.h"

#include <OverEngine/ImGui/ExtraImGui.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <fmt/format.h>

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
		if (m_Context->AnySceneOpen())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
			ImGui::Begin("Scene Hierarchy");
			ImGui::PopStyleVar();

			Entity clickedEntity = RecursiveDraw();

			if (clickedEntity)
				m_Context->Selection = clickedEntity;

			ImGui::BeginChild("Blank Space");
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
					m_Context->Selection.reset();

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 5.0f, 5.0f });
				if (ImGui::BeginPopupContextWindow("_SCENE_HIERARCHY_CONTEXTMENU"))
				{
					if (ImGui::MenuItem("Create Empty Entity"))
						m_Context->Selection = m_Context->GetActiveScene()->CreateEntity();

					ImGui::EndPopup();
				}
				ImGui::PopStyleVar();
			}
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
			ImGui::TextUnformatted("Please open a scene to start editing!");
			ImGui::End();
		}

		ImGui::Begin("Inspector");

		if (m_Context->Selection)
		{
			Entity selectedEntity{ *m_Context->Selection, m_Context->GetActiveScene().get() };

			{
				fmt::basic_memory_buffer<char, 100> buffer;
				*fmt::format_to(buffer, "INSPECTOR_ENTITY_EDITOR{}", selectedEntity.GetRuntimeID()) = '\0';
				ImGui::PushID(buffer.data());

				buffer.clear();
				*fmt::format_to(buffer, "0x{0:x}", selectedEntity.GetComponent<IDComponent>().ID) = '\0';
				ImGui::InputText(buffer.data(), &selectedEntity.GetComponent<NameComponent>().Name);
			}
			
			ImGui::Text("RuntimeID: %i", selectedEntity.GetRuntimeID());

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

//			if (ImGui::Button("Add Component##Button", ImVec2(-1.0f, 40.0f)))
			if (ImGui::Button("Add Component##Button"))
				ImGui::OpenPopup("Add Component##Popup");

			if (ImGui::BeginPopup("Add Component##Popup"))
			{
				CheckAddComponent<TransformComponent>(selectedEntity, "Transform##AddComponentPopup");
				CheckAddComponent<SpriteRendererComponent>(selectedEntity, "SpriteRenderer##AddComponentPopup", nullptr);
				CheckAddComponent<CameraComponent>(selectedEntity, "Camera##AddComponentPopup");
				CheckAddComponent<RigidBody2DComponent>(selectedEntity, "RigidBody2D##AddComponentPopup");
				CheckAddComponent<Colliders2DComponent>(selectedEntity, "Colliders2D##AddComponentPopup");

				ImGui::EndPopup();
			}

			ImGui::PopID();

			if (wannaDestroy)
			{
				selectedEntity.Destroy();
				m_Context->Selection.reset();
			}
		}
		else
		{
			ImGui::TextUnformatted("Please select some thing to edit!");
		}

		ImGui::End();
	}

	Entity SceneHierarchyPanel::RecursiveDraw(Entity parentEntity)
	{
		Entity selectedEntity;

		Ref<Scene> activeScene = m_Context->GetActiveScene();
		auto entityList = parentEntity ? (&parentEntity.GetComponent<TransformComponent>().GetChildrenHandles()) : (&activeScene->GetRootHandles());

		for (auto entityHandle : *entityList)
		{
			Entity entity{ entityHandle, activeScene.get() };

			ImGuiTreeNodeFlags nodeFlags = OE_IMGUI_BASE_TREE_VIEW_FLAGS | ImGuiTreeNodeFlags_AllowItemOverlap;

			if (m_Context->Selection == entity)
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
			ImGui::InvisibleButton("ENTITY_REORDER", { -1, 5 });
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
						else if (other != tc.GetParent())
						{
							uint32_t si = tc.GetSiblingIndex();
							if (otherTc.GetSiblingIndex() > si)
								si++;
							otherTc.SetParent(tc.GetParent());
							otherTc.SetSiblingIndex(si);
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
