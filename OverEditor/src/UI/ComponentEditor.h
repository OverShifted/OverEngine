#pragma once

#include <OverEngine/ImGui/UIElements.h>
#include <OverEngine/Scene/Components.h>
#include <OverEngine/Core/Runtime/Reflection/TypeInfo.h>
#include <imgui/imgui.h>

#define ECS_ACTION_FUNCS(T, setter, getter)       \
	[entity]() -> Variant {                       \
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
		entity.GetComponent<T>().setter((EnumT)std::any_cast<int>(v)); \
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
		UIElements::BeginFieldGroup();

		const auto& props = dynamic_cast<ObjectTypeInfo*>(TypeResolver<T>::Get())->PropertiesOrder;
		for (const auto& prop : props)
		{
			prop->Type->DrawInspectorGUI(prop->InspectorName.c_str(), [entity]() { return (void*)&entity.GetComponent<T>(); }, prop->Getter, prop->Setter, prop->Hint);
		}

		UIElements::EndFieldGroup();

#if 0
		if (UIElements::BeginComponentEditor<T>(entity, entity.GetComponent<T>().GetName(), typeID))
			ImGui::TextUnformatted("No Overloaded function for this component found!");
#endif
	}

	// Overloads
	template<>
	void ComponentEditor<CameraComponent>(Entity entity, uint32_t typeID)
	{
        if (UIElements::BeginComponentEditor<CameraComponent>(entity, "Camera", typeID))
		{
            UIElements::BeginFieldGroup();

            UIElements::Color4Field_U("Clear Color",
                ECS_ACTION_FUNCS(CameraComponent, Camera.SetClearColor, Camera.GetClearColor)
            );

            // TODO: Perspective Camera
            UIElements::DragFloatField_U("Orthographic Size",
				ECS_ACTION_FUNCS(CameraComponent, Camera.SetOrthographicSize, Camera.GetOrthographicSize),
				0.5f, 0.0001f, FLT_MAX
			);

            UIElements::DragFloatField_U("Orthographic NearClip",
				ECS_ACTION_FUNCS(CameraComponent, Camera.SetOrthographicNearClip, Camera.GetOrthographicNearClip), 0.5f
			);

            UIElements::DragFloatField_U("Orthographic FarClip",
				ECS_ACTION_FUNCS(CameraComponent, Camera.SetOrthographicFarClip, Camera.GetOrthographicFarClip), 0.5f
			);

			UIElements::CheckboxField_U("Is Clearing Color",
				ECS_FLAG_ACTION_VALUE(CameraComponent, ClearFlags_ClearColor, Camera.GetClearFlags())
			);

			UIElements::CheckboxField_U("Is Clearing Depth",
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

//            UIElements::BasicEnum_U("Type", typeValues, false, ECS_ENUM_ACTION_FUNCS(RigidBody2DComponent, RigidBody2DType, RigidBody->SetType, RigidBody->GetType));

            UIElements::DragFloat2Field_U("LinearVelocity", ECS_ACTION_FUNCS(RigidBody2DComponent, RigidBody->SetLinearVelocity, RigidBody->GetLinearVelocity));
            UIElements::DragFloatField_U("AngularVelocity", ECS_ACTION_FUNCS(RigidBody2DComponent, RigidBody->SetAngularVelocity, RigidBody->GetAngularVelocity));

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
	void ComponentEditor<Collider2DComponent>(Entity entity, uint32_t typeID)
	{
		if (UIElements::BeginComponentEditor<Collider2DComponent>(entity, "Colliders2D", typeID))
		{
			auto& pcc = entity.GetComponent<Collider2DComponent>();

			static UIElements::EnumValues typeValues = {
				{ 0, "Box" }, { 1, "Circle" }
			};

			// TODO: implement
		}
	}
}
