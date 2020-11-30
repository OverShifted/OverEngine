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
		if (UIElements::BeginComponentEditor<TransformComponent>(entity, "Transform", typeID))
		{
			UIElements::BeginFieldGroup();

			auto& tc = entity.GetComponent<TransformComponent>();
			Vector3 position = tc.GetLocalPosition();
			Vector3 rotation = tc.GetLocalEulerAngles();
			Vector3 scale = tc.GetLocalScale();

			if (UIElements::DragFloat3Field_U("Position", "##Position", glm::value_ptr(position), 
				[entity]() mutable {
					return entity.GetComponent<TransformComponent>().GetLocalPosition();
				},
				[entity](const auto& pos) mutable {
					entity.GetComponent<TransformComponent>().SetLocalPosition(pos);
				}
				, 0.2f))
			{
				tc.SetLocalPosition(position);
			}

			if (UIElements::DragFloat3Field_U("Rotation", "##Rotation", glm::value_ptr(rotation),
				[entity]() mutable {
					return entity.GetComponent<TransformComponent>().GetLocalEulerAngles();
				},
				[entity](const auto& rot) mutable {
					entity.GetComponent<TransformComponent>().SetLocalEulerAngles(rot);
				}
				, 0.2f))
			{
				tc.SetLocalEulerAngles(rotation);
			}

			if (UIElements::DragFloat3Field_U("Scale", "##Scale", glm::value_ptr(scale),
				[entity]() mutable {
					return entity.GetComponent<TransformComponent>().GetLocalScale();
				},
				[entity](const auto& scale) mutable {
					entity.GetComponent<TransformComponent>().SetLocalScale(scale);
				}
				, 0.2f))
			{
				tc.SetLocalScale(scale);
			}

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
		if (UIElements::BeginComponentEditor<SpriteRendererComponent>(entity, "SpriteRenderer", typeID))
		{
			UIElements::BeginFieldGroup();

			auto& sp = entity.GetComponent<SpriteRendererComponent>();

			UIElements::Color4Field_U("Tint", "##Tint", glm::value_ptr(sp.Tint),
				[entity]() mutable {
					return entity.GetComponent<SpriteRendererComponent>().Tint;
				},
				[entity](const auto& col) mutable {
					entity.GetComponent<SpriteRendererComponent>().Tint = col;
				}
			);

			UIElements::DragFloatField_U("AlphaClipThreshold", "##AlphaClipThreshold", &sp.AlphaClipThreshold,
				[entity]() mutable {
					return entity.GetComponent<SpriteRendererComponent>().AlphaClipThreshold;
				},
				[entity](const auto& val) mutable {
					entity.GetComponent<SpriteRendererComponent>().AlphaClipThreshold = val;
				}
				, 0.02f, 0.0f, 1.0f
			);

			UIElements::Texture2DField("Sprite", "##Sprite", sp.Sprite);

			if (sp.Sprite && sp.Sprite->GetType() != TextureType::Placeholder)
			{
				UIElements::CheckboxField("Flip.x", "##Flip.x", &sp.Flip.x);
				UIElements::CheckboxField("Flip.y", "##Flip.y", &sp.Flip.y);

				UIElements::DragFloat2Field("Tiling", "##Tiling", glm::value_ptr(sp.Tiling), 0.02f);
				UIElements::DragFloat2Field("Offset", "##Offset", glm::value_ptr(sp.Offset), 0.02f);

				UIElements::EndFieldGroup();
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				bool open = ImGui::TreeNodeEx("Advance Options##SpriteRendererComponentEditor", ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
				UIElements::BeginFieldGroup();

				if (open)
				{
					static UIElements::EnumValues filteringValues = {
						{ 0, "None (Use default)" }, { 1, "Linear" }, { 2, "Linear" }
					};
					UIElements::BasicEnum("Filtering", "##Filtering", filteringValues, (int8_t*)&sp.Filtering);

					static UIElements::EnumValues wrappingValues = {
						{ 0, "None (Use default)" }, { 1, "Repeat" },
						{ 2, "MirroredRepeat" }, { 3, "ClampToEdge" },{ 4, "ClampToBorder" }
					};
					UIElements::BasicEnum("Wrapping.x", "##Wrapping.x", wrappingValues, (int8_t*)&sp.Wrapping.x);
					UIElements::BasicEnum("Wrapping.y", "##Wrapping.y", wrappingValues, (int8_t*)&sp.Wrapping.y);

					if (sp.Wrapping.x == TextureWrapping::ClampToBorder || sp.Wrapping.y == TextureWrapping::ClampToBorder)
					{
						UIElements::CheckboxField("OverrideTextureBorderColor)",
							"##OverrideTextureBorderColor", &sp.TextureBorderColor.first);

						if (sp.TextureBorderColor.first)
						{
							UIElements::Color4Field("TextureBorderColor)",
								"##BorderColor", glm::value_ptr(sp.TextureBorderColor.second));
						}
					}

					ImGui::TreePop();
				}
			}

			UIElements::EndFieldGroup();
		}
	}

	template <>
	void ComponentEditor<CameraComponent>(Entity entity, uint32_t typeID)
	{
		if (UIElements::BeginComponentEditor<CameraComponent>(entity, "Camera", typeID))
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
		if (UIElements::BeginComponentEditor<RigidBody2DComponent>(entity, "RigidBody2D", typeID))
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
			bool Bullet = false;
			*/

			UIElements::EndFieldGroup();
		}
	}

	template <>
	void ComponentEditor<Colliders2DComponent>(Entity entity, uint32_t typeID)
	{
		if (UIElements::BeginComponentEditor<Colliders2DComponent>(entity, "Colliders2D", typeID))
		{
			auto& pcc = entity.GetComponent<Colliders2DComponent>();

			static UIElements::EnumValues typeValues = {
				{ 0, "Box" }, { 1, "Circle" }
			};

			auto colliderIt = pcc.Colliders.begin();
			while (colliderIt != pcc.Colliders.end())
			{
				ImGui::PushID(&(*colliderIt));

				if (ImGui::Button("X"))
				{
					colliderIt = pcc.Colliders.erase(colliderIt);
				}
				else
				{
					auto& collider = *colliderIt;

					ImGui::SameLine();
					ImGui::SetNextItemOpen(true, ImGuiCond_Once);
					if (ImGui::TreeNodeEx(&collider, ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap, "%s", "Collider2D"))
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

					colliderIt++;
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
