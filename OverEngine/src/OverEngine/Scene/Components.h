#pragma once

#include "Entity.h"

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/GUIDGenerator.h"

#include "OverEngine/Assets/Asset.h"

#include "OverEngine/Physics/PhysicsBody2D.h"
#include "OverEngine/Physics/PhysicsCollider2D.h"

#include "OverEngine/Scene/SceneCamera.h"

namespace OverEngine
{
	class Scene;

	enum class ComponentType
	{
		BaseComponent, TransformComponent, CameraComponent,
		SpriteRendererComponent, PhysicsBody2DComponent,
		PhysicsColliders2DComponent
	};

	#define COMPONENT_TYPE(type) static ComponentType GetStaticType() { return ComponentType::type; }\
							virtual ComponentType GetComponentType() const override { return GetStaticType(); }\
							virtual const char* GetName() const override { return #type; }

	struct Component
	{
		Component(Entity attachedEntity, bool enabled = true)
			: AttachedEntity(attachedEntity), Enabled(enabled) {}

		Entity AttachedEntity;
		bool Enabled = true;

		virtual ComponentType GetComponentType() const = 0;
		virtual const char* GetName() const = 0;
	};

	////////////////////////////////////////////////////////
	// Common Components ///////////////////////////////////
	////////////////////////////////////////////////////////

	struct BaseComponent : public Component
	{
		String Name;
		Guid ID = GUIDGenerator::GenerateVersion4();

		Entity Parent;
		Vector<Entity> Children;

		BaseComponent() = default;
		BaseComponent(const BaseComponent&) = default;
		BaseComponent(Entity& entity, const String& name, Entity parent = Entity())
			: Component(entity), Name(name), Parent(parent)
		{
			if (parent)
				parent.GetComponent<BaseComponent>().Children.push_back(entity);
		}

		COMPONENT_TYPE(BaseComponent)
	};

	struct TransformComponent : public Component
	{
		OverEngine::Transform Transform;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		TransformComponent(Entity& entity, const OverEngine::Transform& transform)
			: Component(entity), Transform(transform) {}

		TransformComponent(Entity& entity)
			: Component(entity) {}

		operator Mat4x4() const { return Transform.GetMatrix(); }
		operator const Mat4x4&() { return Transform.GetMatrix(); }
		const OverEngine::Transform* operator->() const { return &Transform; }
		OverEngine::Transform* operator->() { return &Transform; }

		COMPONENT_TYPE(TransformComponent)
	};

	////////////////////////////////////////////////////////
	// Renderer Components /////////////////////////////////
	////////////////////////////////////////////////////////

	struct CameraComponent : public Component
	{
		SceneCamera Camera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		CameraComponent(Entity& entity, const SceneCamera& camera)
			: Component(entity), Camera(camera) {}

		CameraComponent(Entity& entity)
			: Component(entity) {}

		COMPONENT_TYPE(CameraComponent)
	};

	struct SpriteRendererComponent : public Component
	{
		Ref<Texture2DAsset> Sprite;
		Color Tint{ 1.0f, 1.0f, 1.0f, 1.0f };
		float TilingFactorX = 1.0f;
		float TilingFactorY = 1.0f;
		bool FlipX = false, FlipY = false;
		TextureWrapping OverrideSWrapping = TextureWrapping::None;
		TextureWrapping OverrideTWrapping = TextureWrapping::None;
		TextureFiltering OverrideFiltering = TextureFiltering::None;
		float AlphaClippingThreshold = 0.0f;
		bool IsOverrideTextureBorderColor = false;
		Color OverrideTextureBorderColor = Color(1);

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		SpriteRendererComponent(Entity& entity, Ref<Texture2DAsset> sprite)
			: Component(entity), Sprite(sprite) {}

		SpriteRendererComponent(Entity& entity, Ref<Texture2DAsset> sprite, const Color& tint)
			: Component(entity), Sprite(sprite), Tint(tint) {}

		COMPONENT_TYPE(SpriteRendererComponent)
	};

	////////////////////////////////////////////////////////
	// Physics Components //////////////////////////////////
	////////////////////////////////////////////////////////

	struct PhysicsBody2DComponent : public Component
	{
		Ref<PhysicsBody2D> Body;

		PhysicsBody2DComponent() = default;
		PhysicsBody2DComponent(const PhysicsBody2DComponent&) = default;

		PhysicsBody2DComponent(Entity& entity, const PhysicsBodyProps& props);

		COMPONENT_TYPE(PhysicsBody2DComponent)
	};

	struct PhysicsColliders2DComponent : public Component
	{
		Vector<Ref<PhysicsCollider2D>> Colliders;
		Ref<PhysicsBody2D> AttachedBody;

		PhysicsColliders2DComponent() = default;
		PhysicsColliders2DComponent(const PhysicsColliders2DComponent&) = default;

		PhysicsColliders2DComponent(Entity& entity)
			: Component(entity)
		{
			if (entity.HasComponent<PhysicsBody2DComponent>())
				AttachedBody = entity.GetComponent<PhysicsBody2DComponent>().Body;
			else
				AttachedBody = nullptr;
		}

		void AddCollider(const Ref<PhysicsCollider2D>& collider)
		{
			Colliders.push_back(collider);

			if (AttachedBody)
				AttachedBody->AddCollider(collider);
		}

		void RemoveCollider(const Ref<PhysicsCollider2D>& collider)
		{
			auto it = std::find(Colliders.begin(), Colliders.end(), collider);
			if (it != Colliders.end())
			{
				if (AttachedBody)
					AttachedBody->RemoveCollider(collider);
				Colliders.erase(it);
			}
			else
				OE_CORE_ASSERT(false, "Collider dosen't exist");
		}

		bool HasCollider(const Ref<PhysicsCollider2D>& collider)
		{
			auto it = std::find(Colliders.begin(), Colliders.end(), collider);
			return it != Colliders.end();
		}

		COMPONENT_TYPE(PhysicsColliders2DComponent)
	};
}
