#pragma once

#include "UIElements.h"
#include "EditorLayer.h"

#include <OverEngine/Scene/Components.h>
#include <imgui/imgui.h>


#define __BASIC_ACTION_FUNCS(T, setter, getter)   \
	[entity]() {                                  \
		return entity.GetComponent<T>().getter(); \
	},                                            \
	[entity](const auto& v) {                     \
		entity.GetComponent<T>().setter(v);       \
	}

#define __BASIC_ACTION_VAL(T, val)                \
	[entity]() {                                  \
		return entity.GetComponent<T>().val;      \
	},                                            \
	[entity](const auto& v) {                     \
		entity.GetComponent<T>().val = v;         \
	}

#define __BASIC_ENUM_ACTION_VAL(T, EnumT, val)    \
	[entity]() {                                  \
		return (int)entity.GetComponent<T>().val; \
	},                                            \
	[entity](const auto& v) {                     \
		entity.GetComponent<T>().val = (EnumT)v;  \
	}

#define __BASIC_FLAG_ACTION(T, FlagT, flag, refrence_getter)              \
	(FlagT*)&entity.GetComponent<T>().refrence_getter, (FlagT)flag,       \
	[entity]() {                                                          \
		return (entity.GetComponent<T>().refrence_getter & flag) == flag; \
	},                                                                    \
	[entity](const auto& v) {                                             \
		if (v) entity.GetComponent<T>().refrence_getter |= flag;          \
		else entity.GetComponent<T>().refrence_getter &= ~flag;           \
	}

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
				__BASIC_ACTION_FUNCS(TransformComponent, SetLocalPosition, GetLocalPosition)
				, 0.2f))
			{
				tc.SetLocalPosition(position);
			}

			if (UIElements::DragFloat3Field_U("Rotation", "##Rotation", glm::value_ptr(rotation),
				__BASIC_ACTION_FUNCS(TransformComponent, SetLocalEulerAngles, GetLocalEulerAngles)
				, 0.2f))
			{
				tc.SetLocalEulerAngles(rotation);
			}

			if (UIElements::DragFloat3Field_U("Scale", "##Scale", glm::value_ptr(scale),
				__BASIC_ACTION_FUNCS(TransformComponent, SetLocalScale, GetLocalScale)
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
				__BASIC_ACTION_VAL(SpriteRendererComponent, Tint)
			);

			UIElements::Texture2DField("Sprite", "##Sprite", sp.Sprite);

			if (sp.Sprite && !sp.Sprite->IsRefrence())
			{
				UIElements::CheckboxFlagsField_U("Flip.x", "##Flip.x",
					__BASIC_FLAG_ACTION(SpriteRendererComponent, uint8_t, TextureFlip_X, Flip)
				);
				UIElements::CheckboxFlagsField_U("Flip.y", "##Flip.y",
					__BASIC_FLAG_ACTION(SpriteRendererComponent, uint8_t, TextureFlip_Y, Flip)
				);

				UIElements::DragFloat2Field_U("Tiling", "##Tiling", glm::value_ptr(sp.Tiling),
					__BASIC_ACTION_VAL(SpriteRendererComponent, Tiling), 0.02f
				);
				UIElements::DragFloat2Field_U("Offset", "##Offset", glm::value_ptr(sp.Offset),
					__BASIC_ACTION_VAL(SpriteRendererComponent, Offset), 0.02f
				);

				UIElements::EndFieldGroup();

				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				bool open = ImGui::TreeNodeEx("Advance Options##SpriteRendererComponentEditor",
					ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap
				);

				UIElements::BeginFieldGroup();

				if (open)
				{
					static UIElements::EnumValues filterValues = {
						{ 0, "None (Use default)" }, { 1, "Nearest" }, { 2, "Linear" }
					};
					UIElements::BasicEnum("Filter", "##Filter", filterValues, (int8_t*)&sp.Filter);

					static UIElements::EnumValues wrapValues = {
						{ 0, "None (Use default)" }, { 1, "Repeat" },
						{ 2, "Clamp" }, { 3, "Mirror" }
					};
					UIElements::BasicEnum_U("Wrap.x", "##Wrap.x", wrapValues, (int8_t*)&sp.Wrap.x,
						__BASIC_ENUM_ACTION_VAL(SpriteRendererComponent, TextureWrap, Wrap.x)
					);
					UIElements::BasicEnum_U("Wrap.y", "##Wrap.y", wrapValues, (int8_t*)&sp.Wrap.y,
						__BASIC_ENUM_ACTION_VAL(SpriteRendererComponent, TextureWrap, Wrap.y)
					);

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
			if (UIElements::DragFloatField_U("Orthographic Size", "##Orthographic Size", &orthoSize,
				__BASIC_ACTION_FUNCS(CameraComponent, Camera.SetOrthographicSize, Camera.GetOrthographicSize),
				0.5f, 0.0001f, FLT_MAX))
			{
				camera.Camera.SetOrthographicSize(orthoSize);
			}

			if (UIElements::DragFloatField_U("OrthographicNearClip", "##OrthographicNearClip", &OrthographicNearClip,
				__BASIC_ACTION_FUNCS(CameraComponent, Camera.SetOrthographicNearClip, Camera.GetOrthographicNearClip), 0.5f))
			{
				camera.Camera.SetOrthographicNearClip(OrthographicNearClip);
			}

			if (UIElements::DragFloatField_U("OrthographicFarClip", "##OrthographicFarClip", &OrthographicFarClip,
				__BASIC_ACTION_FUNCS(CameraComponent, Camera.SetOrthographicFarClip, Camera.GetOrthographicFarClip), 0.5f))
			{
				camera.Camera.SetOrthographicFarClip(OrthographicFarClip);
			}

			if (UIElements::Color4Field_U("Clear Color", "##Clear Color", glm::value_ptr(clearColor),
				__BASIC_ACTION_FUNCS(CameraComponent, Camera.SetClearColor, Camera.GetClearColor)))
			{
				camera.Camera.SetClearColor(clearColor);
			}

			UIElements::CheckboxFlagsField_U("Is Clearing Color", "##Is Clearing Color",
				__BASIC_FLAG_ACTION(CameraComponent, uint8_t, ClearFlags_ClearColor, Camera.GetClearFlags()));

			UIElements::CheckboxFlagsField_U("Is Clearing Depth", "##Is Clearing Depth",
				__BASIC_FLAG_ACTION(CameraComponent, uint8_t, ClearFlags_ClearDepth, Camera.GetClearFlags()));

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
								{
									if (radius < 0.001f)
										radius = 0.001f;

									collider.Collider->ReShape(radius);
								}
							}
							else if (collider.Initializer.Shape.Type == Collider2DType::Box)
							{
								auto size = collider.Collider->GetSizeHint();
								if (UIElements::DragFloat2Field("Size", "##Size", glm::value_ptr(size)))
								{
									if (size.x < 0.001f)
										size.x = 0.001f;
									if (size.y < 0.001f)
										size.y = 0.001f;

									collider.Collider->ReShape(size);
								}
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
