#pragma once

#include "UIElements.h"
#include "EditorLayer.h"

#include <OverEngine/Scene/Components.h>
#include <imgui/imgui.h>


#define ECS_ACTION_FUNCS(T, setter, getter)       \
	[entity]() {                                  \
		return entity.GetComponent<T>().getter(); \
	},                                            \
	[entity](const auto& v) {                     \
		entity.GetComponent<T>().setter(v);       \
	}

#define ECS_ACTION_VALUE(T, val)             \
	[entity]() {                             \
		return entity.GetComponent<T>().val; \
	},                                       \
	[entity](const auto& v) {                \
		entity.GetComponent<T>().val = v;    \
	}

#define ECS_ENUM_ACTION_FUNCS(T, EnumT, setter, getter) \
	[entity]() {                                        \
		return (int)entity.GetComponent<T>().getter();  \
	},                                                  \
	[entity](const auto& v) {                           \
		entity.GetComponent<T>().setter((EnumT)v);      \
	}

#define ECS_FLAG_ACTION_VALUE(T, flag, reference_getter)                   \
	[entity]() {                                                           \
		return (entity.GetComponent<T>().reference_getter & flag) == flag; \
	},                                                                     \
	[entity](const auto& v) {                                              \
		if (v) entity.GetComponent<T>().reference_getter |= flag;          \
		else entity.GetComponent<T>().reference_getter &= ~flag;           \
	}

namespace OverEditor
{
	// Base
	template<typename T>
	void ComponentEditor(Entity entity, uint32_t typeID)
	{
		if (UIElements::BeginComponentEditor<T>(entity, entity.GetComponent<T>().GetName(), typeID))
			ImGui::TextUnformatted("No Overloaded function for this component found!");
	}

	// Overloads
	template<>
	void ComponentEditor<TransformComponent>(Entity entity, uint32_t typeID)
	{
		if (UIElements::BeginComponentEditor<TransformComponent>(entity, "Transform", typeID))
		{
			UIElements::BeginFieldGroup();

			UIElements::DragFloat3Field_U("Position", "##Position",
			    ECS_ACTION_FUNCS(TransformComponent, SetLocalPosition, GetLocalPosition)
			    , 0.2f
            );

			UIElements::DragFloat3Field_U("Rotation", "##Rotation",
				ECS_ACTION_FUNCS(TransformComponent, SetLocalEulerAngles, GetLocalEulerAngles)
				, 0.2f
            );

			UIElements::DragFloat3Field_U("Scale", "##Scale",
				ECS_ACTION_FUNCS(TransformComponent, SetLocalScale, GetLocalScale)
				, 0.2f
            );

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

	template<>
	void ComponentEditor<SpriteRendererComponent>(Entity entity, uint32_t typeID)
	{
		if (UIElements::BeginComponentEditor<SpriteRendererComponent>(entity, "SpriteRenderer", typeID))
		{
			UIElements::BeginFieldGroup();

			auto& sp = entity.GetComponent<SpriteRendererComponent>();

			UIElements::Color4Field_U("Tint", "##Tint",
				ECS_ACTION_VALUE(SpriteRendererComponent, Tint)
			);

			UIElements::Texture2DField("Sprite", "##Sprite", sp.Sprite);

			if (sp.Sprite && !sp.Sprite->IsRefrence())
			{
				UIElements::CheckboxField_U("Flip.x", "##Flip.x",
					ECS_FLAG_ACTION_VALUE(SpriteRendererComponent, TextureFlip_X, Flip)
				);
				UIElements::CheckboxField_U("Flip.y", "##Flip.y",
					ECS_FLAG_ACTION_VALUE(SpriteRendererComponent, TextureFlip_Y, Flip)
				);

				UIElements::DragFloat2Field_U("Tiling", "##Tiling",
					ECS_ACTION_VALUE(SpriteRendererComponent, Tiling), 0.02f
				);
				UIElements::DragFloat2Field_U("Offset", "##Offset",
					ECS_ACTION_VALUE(SpriteRendererComponent, Offset), 0.02f
				);

				UIElements::CheckboxField_U("ForceTile", "##ForceTile",
					ECS_ACTION_VALUE(SpriteRendererComponent, ForceTile)
				);
			}

			UIElements::EndFieldGroup();
		}
	}

	template<>
	void ComponentEditor<CameraComponent>(Entity entity, uint32_t typeID)
	{
        if (UIElements::BeginComponentEditor<CameraComponent>(entity, "Camera", typeID))
		{
            UIElements::BeginFieldGroup();

            UIElements::Color4Field_U("Clear Color", "##Clear Color",
                ECS_ACTION_FUNCS(CameraComponent, Camera.SetClearColor, Camera.GetClearColor)
            );

            // TODO: Perspective Camera
            UIElements::DragFloatField_U("Orthographic Size", "##Orthographic Size",
				ECS_ACTION_FUNCS(CameraComponent, Camera.SetOrthographicSize, Camera.GetOrthographicSize),
				0.5f, 0.0001f, FLT_MAX
			);

            UIElements::DragFloatField_U("OrthographicNearClip", "##OrthographicNearClip",
				ECS_ACTION_FUNCS(CameraComponent, Camera.SetOrthographicNearClip, Camera.GetOrthographicNearClip), 0.5f
			);

            UIElements::DragFloatField_U("OrthographicFarClip", "##OrthographicFarClip",
				ECS_ACTION_FUNCS(CameraComponent, Camera.SetOrthographicFarClip, Camera.GetOrthographicFarClip), 0.5f
			);

			UIElements::CheckboxField_U("Is Clearing Color", "##Is Clearing Color",
				ECS_FLAG_ACTION_VALUE(CameraComponent, ClearFlags_ClearColor, Camera.GetClearFlags())
			);

			UIElements::CheckboxField_U("Is Clearing Depth", "##Is Clearing Depth",
				ECS_FLAG_ACTION_VALUE(CameraComponent, ClearFlags_ClearDepth, Camera.GetClearFlags())
			);

			UIElements::EndFieldGroup();
		}
	}

	template<>
	void ComponentEditor<RigidBody2DComponent>(Entity entity, uint32_t typeID)
	{

		if (UIElements::BeginComponentEditor<RigidBody2DComponent>(entity, "RigidBody2D", typeID))
		{
			UIElements::BeginFieldGroup();
			
			static UIElements::EnumValues typeValues = {
				{ 1, "Static" }, { 2, "Kinematic" }, { 3,"Dynamic" }
			};

            UIElements::BasicEnum_U("Type", "##Type", typeValues, ECS_ENUM_ACTION_FUNCS(RigidBody2DComponent, RigidBody2DType, RigidBody->SetType, RigidBody->GetType));

            UIElements::DragFloat2Field_U("LinearVelocity", "##LinearVelocity", ECS_ACTION_FUNCS(RigidBody2DComponent, RigidBody->SetLinearVelocity, RigidBody->GetLinearVelocity));
            UIElements::DragFloatField_U("AngularVelocity", "##AngularVelocity", ECS_ACTION_FUNCS(RigidBody2DComponent, RigidBody->SetAngularVelocity, RigidBody->GetAngularVelocity));

//			UIElements::DragFloatField_U("LinearDamping", "##LinearDamping", ECS_ACTION_FUNCS(RigidBody2DComponent, RigidBody.Linea));
//			UIElements::DragFloatField("AngularDamping", "##AngularDamping", &rbc.Initializer.AngularDamping);

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

	template<>
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
//						UIElements::BeginFieldGroup();
//
//						// TODO: change settings in realtime
//
//						UIElements::DragFloat2Field("Offset", "##Offset", glm::value_ptr(collider.Initializer.Offset));
//
//						Vector2 offset = collider->GetOffset();
//
//						if (UIElements::DragFloat2Field_U("Offset", "##Offset", &offset,
//							ECS_ACTION_FUNCS(CameraComponent, Camera.SetOrthographicFarClip, Camera.GetOrthographicFarClip), 0.5f))
//						{
//							camera.Camera.SetOrthographicFarClip(OrthographicFarClip);
//						}
//
//						if (collider.Initializer.Shape.Type == Collider2DType::Circle)
//						{
//							UIElements::DragFloatField("Radius", "##Radius", &collider.Initializer.Shape.CircleRadius);
//						}
//						else if (collider.Initializer.Shape.Type == Collider2DType::Box)
//						{
//							UIElements::DragFloat2Field("Size", "##Size", glm::value_ptr(collider.Initializer.Shape.BoxSize));
//						}
//
//						{
//							Vector2 offset = collider.Collider->GetOffset();
//							if (UIElements::DragFloat2Field("Offset", "##Offset", glm::value_ptr(offset)))
//								collider.Collider->SetOffset(offset);
//
//							float rotation = collider.Collider->GetRotation();
//							if (UIElements::DragFloatField("Rotation", "##Rotation", &rotation))
//								collider.Collider->SetRotation(rotation);
//
//							if (collider.Initializer.Shape.Type == Collider2DType::Circle)
//							{
//								float radius = collider.Collider->GetSizeHint().x;
//								if (UIElements::DragFloatField("Radius", "##Radius", &radius))
//								{
//									if (radius < 0.001f)
//										radius = 0.001f;
//
//									collider.Collider->ReShape(radius);
//								}
//							}
//							else if (collider.Initializer.Shape.Type == Collider2DType::Box)
//							{
//								auto size = collider.Collider->GetSizeHint();
//								if (UIElements::DragFloat2Field("Size", "##Size", glm::value_ptr(size)))
//								{
//									if (size.x < 0.001f)
//										size.x = 0.001f;
//									if (size.y < 0.001f)
//										size.y = 0.001f;
//
//									collider.Collider->ReShape(size);
//								}
//							}
//						}
//
//						UIElements::EndFieldGroup();
						ImGui::TreePop();
					}

					colliderIt++;
				}

				ImGui::PopID();
			}

			if (ImGui::Button("Add Collider", { -1, 0 }))
			{
				Collider2DProps props;
				props.Shape = BoxCollisionShape2D::Create({ 1.0f, 1.0f });
				pcc.Colliders.push_back(Collider2D::Create(props));
			}
		}
	}
}
