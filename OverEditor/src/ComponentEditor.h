#pragma once

#include "UIElements.h"
#include "EditorLayer.h"

#include <OverEngine/Scene/Components.h>
#include <imgui/imgui.h>

namespace OverEditor
{
	// Base
	template <class T>
	void ComponentEditor(Entity entity, uint32_t typeID)
	{
		if (UIElements::BeginComponentEditor<T>(entity, entity.GetComponent<T>().GetName(), typeID))
			ImGui::TextUnformatted("No Overloaded function for this component found!");
	}

	// Overloads
	template <>
	void ComponentEditor<TransformComponent>(Entity entity, uint32_t typeID)
	{
		if (UIElements::BeginComponentEditor<TransformComponent>(entity, "Transform Component", typeID))
		{
			UIElements::BeginFieldGroup();

			static Vector3 positionDelta(0.0f);
			static Vector3 rotationDelta(0.0f);
			static Vector3 scaleDelta(0.0f);

			auto& tc = entity.GetComponent<TransformComponent>();
			Vector3 position = tc.GetLocalPosition();
			Vector3 rotation = tc.GetLocalEulerAngles();
			Vector3 scale = tc.GetLocalScale();

			bool changed = false;
			if (UIElements::DragFloat3Field("Position", "##Position", glm::value_ptr(position), 0.2f))
			{
				positionDelta += position - tc.GetLocalPosition();
				changed = true;
			}

			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				auto action = CreateRef<Vector3EditAction>(positionDelta, [entity]() mutable {
					return entity.GetComponent<TransformComponent>().GetLocalPosition();
				}, [entity](const auto& pos) mutable {
					entity.GetComponent<TransformComponent>().SetLocalPosition(pos);
				});

				EditorLayer::Get().GetActionStack().Do(action, false);
				positionDelta = Vector3(0.0f);
			}

			if (changed)
				tc.SetLocalPosition(position);

			changed = false;
			if (UIElements::DragFloat3Field("Rotation", "##Rotation", glm::value_ptr(rotation), 0.2f))
			{
				rotationDelta += rotation - tc.GetLocalEulerAngles();
				changed = true;
			}

			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				auto action = CreateRef<Vector3EditAction>(rotationDelta, [entity]() mutable {
					return entity.GetComponent<TransformComponent>().GetEulerAngles();
				}, [entity](const auto& rot) mutable {
					entity.GetComponent<TransformComponent>().SetEulerAngles(rot);
				});

				EditorLayer::Get().GetActionStack().Do(action, false);
				rotationDelta = Vector3(0.0f);
			}

			if (changed)
				tc.SetLocalEulerAngles(rotation);

			changed = false;
			if (UIElements::DragFloat3Field("Scale", "##Scale", glm::value_ptr(scale), 0.2f))
			{
				scaleDelta += scale - tc.GetLocalScale();
				changed = true;
			}

			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				auto action = CreateRef<Vector3EditAction>(scaleDelta, [entity]() mutable {
					return entity.GetComponent<TransformComponent>().GetLocalScale();
				}, [entity](const auto& scale) mutable {
					entity.GetComponent<TransformComponent>().SetLocalScale(scale);
				});

				EditorLayer::Get().GetActionStack().Do(action, false);
				scaleDelta = Vector3(0.0f);
			}

			if (changed)
				tc.SetLocalScale(scale);

			UIElements::EndFieldGroup();

			#if 0
			ImGui::Text("%i", transform.GetChangedFlags());

			ImGui::PushItemWidth(-1);
			ImGui::InputFloat4("", (float*)&tc.GetLocalToWorld()[0].x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat4("", (float*)&tc.GetLocalToWorld()[1].x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat4("", (float*)&tc.GetLocalToWorld()[2].x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat4("", (float*)&tc.GetLocalToWorld()[3].x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::PopItemWidth();
			#endif
		}
	}

	template <>
	void ComponentEditor<SpriteRendererComponent>(Entity entity, uint32_t typeID)
	{
		if (UIElements::BeginComponentEditor<SpriteRendererComponent>(entity, "SpriteRenderer Component", typeID))
		{
			UIElements::BeginFieldGroup();

			auto& sp = entity.GetComponent<SpriteRendererComponent>();

			Color tint = sp.Tint;
			static Color tintDelta(0.0f);

			if (UIElements::Color4Field("Tint", "##Tint", glm::value_ptr(tint)))
				tintDelta += tint - sp.Tint;

			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				auto action = CreateRef<Vector4EditAction>(tintDelta, [entity]() mutable {
					return entity.GetComponent<SpriteRendererComponent>().Tint;
				}, [entity](const auto& col) mutable {
					entity.GetComponent<SpriteRendererComponent>().Tint = col;
				});

				EditorLayer::Get().GetActionStack().Do(action, false);
				tintDelta = Vector4(0.0f);
			}

			sp.Tint = tint;

			UIElements::DragFloatField("AlphaClipThreshold", "##AlphaClipThreshold", &sp.AlphaClipThreshold, 0.02f, 0.0f, 1.0f);
			UIElements::Texture2DField("Sprite", "##Sprite", sp.Sprite);

			if (sp.Sprite && sp.Sprite->GetType() != TextureType::Placeholder)
			{
				UIElements::CheckboxField("Flip.x", "##Flip.x", &sp.Flip.x);
				UIElements::CheckboxField("Flip.y", "##Flip.y", &sp.Flip.y);

				UIElements::DragFloat2Field("Tiling", "##Tiling", glm::value_ptr(sp.Tiling), 0.02f);
				UIElements::DragFloat2Field("Offset", "##Offset", glm::value_ptr(sp.Offset), 0.02f);

				static UIElements::EnumValues wrappingValues = {
					{ 0, "None (Use texture default value)" }, { 1, "Repeat" },
					{ 2, "MirroredRepeat" }, { 3, "ClampToEdge" },{ 4, "ClampToBorder" }
				};
				UIElements::BasicEnum("Wrapping.x", "##Wrapping.x", wrappingValues, (int8_t*)&sp.Wrapping.x);
				UIElements::BasicEnum("Wrapping.y", "##Wrapping.y", wrappingValues, (int8_t*)&sp.Wrapping.y);

				static UIElements::EnumValues filteringValues = {
					{ 0, "None (Use texture default value)" }, { 1, "Point" }, { 2, "Linear" }
				};
				UIElements::BasicEnum("Filtering", "##Filtering", filteringValues, (int8_t*)&sp.Filtering);

				UIElements::CheckboxField("OverrideTextureBorderColor (for ClampToBorder wrapping)",
					"##OverrideTextureBorderColor", &sp.TextureBorderColor.first);

				if (sp.TextureBorderColor.first)
					UIElements::Color4Field("TextureBorderColor (for ClampToBorder wrapping)",
						"##BorderColor", glm::value_ptr(sp.TextureBorderColor.second));
			}

			UIElements::EndFieldGroup();
		}
	}

	template <>
	void ComponentEditor<CameraComponent>(Entity entity, uint32_t typeID)
	{
		if (UIElements::BeginComponentEditor<CameraComponent>(entity, "Camera Component", typeID))
		{
			UIElements::BeginFieldGroup();

			auto& camera = entity.GetComponent<CameraComponent>();

			float orthoSize = camera.Camera.GetOrthographicSize();

			Color clearColor = camera.Camera.GetClearColor();

			float OrthographicNearClip = camera.Camera.GetOrthographicNearClip();
			float OrthographicFarClip = camera.Camera.GetOrthographicFarClip();

			// TODO: Perspective Camera
			if (UIElements::DragFloatField("Orthographic Size", "##Orthographic Size", &orthoSize, 0.5f, 0.0001f, FLT_MAX))
				camera.Camera.SetOrthographicSize(orthoSize);

			if (UIElements::DragFloatField("OrthographicNearClip", "##OrthographicNearClip", &OrthographicNearClip, 0.5f))
				camera.Camera.SetOrthographicNearClip(OrthographicNearClip);
			if (UIElements::DragFloatField("OrthographicFarClip", "##OrthographicFarClip", &OrthographicFarClip, 0.5f))
				camera.Camera.SetOrthographicFarClip(OrthographicFarClip);

			if (UIElements::Color4Field("Clear Color", "##Clear Color", glm::value_ptr(clearColor)))
				camera.Camera.SetClearColor(clearColor);

			UIElements::CheckboxFlagsField<uint8_t>("Is Clearing Color", "##Is Clearing Color", (uint8_t*)&camera.Camera.GetClearFlags(), ClearFlags_ClearColor);
			UIElements::CheckboxFlagsField<uint8_t>("Is Clearing Depth", "##Is Clearing Depth", (uint8_t*)&camera.Camera.GetClearFlags(), ClearFlags_ClearDepth);

			UIElements::EndFieldGroup();
		}
	}
	
	template <>
	void ComponentEditor<RigidBody2DComponent>(Entity entity, uint32_t typeID)
	{
		if (UIElements::BeginComponentEditor<RigidBody2DComponent>(entity, "RigidBody2D Component", typeID))
		{
			auto& rbc = entity.GetComponent<RigidBody2DComponent>();

			UIElements::BeginFieldGroup();
			
			static UIElements::EnumValues typeValues = {
				{ 0, "Static" }, { 1, "Kinematic" }, { 2,"Dynamic" }
			};

			if (!rbc.RigidBody)
			{
				UIElements::BasicEnum("Type", "##Type", typeValues, (uint8_t*)&rbc.Initializer.Type);

				UIElements::DragFloat2Field("LinearVelocity", "##LinearVelocity", glm::value_ptr(rbc.Initializer.LinearVelocity));
				UIElements::DragFloatField("AngularVelocity", "##AngularVelocity", &rbc.Initializer.AngularVelocity);

				UIElements::DragFloatField("LinearDamping", "##LinearDamping", &rbc.Initializer.LinearDamping);
				UIElements::DragFloatField("AngularDamping", "##AngularDamping", &rbc.Initializer.AngularDamping);

			}
			else // Simulation Running?
			{
				auto type = rbc.RigidBody->GetType();
				if (UIElements::BasicEnum("Type", "##Type", typeValues, (uint8_t*)&type))
					rbc.RigidBody->SetType(type);

				if (type != RigidBody2DType::Static)
				{
					auto linearVelocity = rbc.RigidBody->GetLinearVelocity();
					if (UIElements::DragFloat2Field("LinearVelocity", "##LinearVelocity", glm::value_ptr(linearVelocity)))
						rbc.RigidBody->SetLinearVelocity(linearVelocity);

					auto angularVelocity = rbc.RigidBody->GetAngularVelocity();
					if (UIElements::DragFloatField("AngularVelocity", "##AngularVelocity", &angularVelocity))
						rbc.RigidBody->SetAngularVelocity(angularVelocity);
				}
			}

			/*
			float LinearDamping = 0.0f;
			float AngularDamping = 0.0f;

			bool AllowSleep = true;
			bool Awake = true;
			bool Enabled = true;

			bool FixedRotation = false;
			float GravityScale = 1.0f;
			bool Bullet = false;*/

			UIElements::EndFieldGroup();
		}
	}

	template <>
	void ComponentEditor<Colliders2DComponent>(Entity entity, uint32_t typeID)
	{
		if (UIElements::BeginComponentEditor<Colliders2DComponent>(entity, "Colliders2D Component", typeID))
		{
			auto& pcc = entity.GetComponent<Colliders2DComponent>();

			static UIElements::EnumValues typeValues = {
				{ 0, "Box" }, { 1, "Circle" }
			};

			for (auto& collider : pcc.Colliders)
			{
				ImGui::PushID(&collider);

				ImGui::Button("X");
				ImGui::SameLine();
				bool open = ImGui::TreeNodeEx(&collider, ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap, "Collider2D");

				if (open)
				{
					UIElements::BeginFieldGroup();
					
					UIElements::BasicEnum("Type", "##Type", typeValues, (int*)&collider.Initializer.Shape.Type, collider.Collider ? ImGuiSelectableFlags_Disabled : 0);

					if (!collider.Collider)
					{
						UIElements::DragFloat2Field("Offset", "##Offset", glm::value_ptr(collider.Initializer.Offset));
						UIElements::DragFloatField("Rotation", "##Rotation", &collider.Initializer.Rotation);

						if (collider.Initializer.Shape.Type == Collider2DType::Circle)
						{
							UIElements::DragFloatField("Radius", "##Radius", &collider.Initializer.Shape.CircleRadius);
						}
						else if (collider.Initializer.Shape.Type == Collider2DType::Box)
						{
							UIElements::DragFloat2Field("Size", "##Size", glm::value_ptr(collider.Initializer.Shape.BoxSize));
						}
					}
					else
					{
						Vector2 offset = collider.Collider->GetOffset();
						if (UIElements::DragFloat2Field("Offset", "##Offset", glm::value_ptr(offset)))
							collider.Collider->SetOffset(offset);

						float rotation = collider.Collider->GetRotation();
						if (UIElements::DragFloatField("Rotation", "##Rotation", &rotation))
							collider.Collider->SetRotation(rotation);

						if (collider.Initializer.Shape.Type == Collider2DType::Circle)
						{
							float radius = collider.Collider->GetSizeHint().x;
							if (UIElements::DragFloatField("Radius", "##Radius", &radius))
								collider.Collider->ReShape(radius);
						}
						else if (collider.Initializer.Shape.Type == Collider2DType::Box)
						{
							auto size = collider.Collider->GetSizeHint();
							if (UIElements::DragFloat2Field("Size", "##Size", glm::value_ptr(size)))
								collider.Collider->ReShape(size);
						}
					}

					UIElements::EndFieldGroup();
					ImGui::TreePop();
				}
				ImGui::PopID();
			}

			if (ImGui::Button("Add Collider", { -1, 0 }))
			{
				Collider2DProps props;
				props.Shape.Type = Collider2DType::Box;
				props.Shape.BoxSize = { 1.0f, 1.0f };
				pcc.Colliders.push_back({ props, nullptr });
			}
		}
	}
}
