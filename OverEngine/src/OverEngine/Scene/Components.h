#pragma once

#include "Entity.h"

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Serialization/Serializer.h"
#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/Random.h"

#include "OverEngine/Physics/PhysicsBody2D.h"
#include "OverEngine/Renderer/Texture.h"
#include "OverEngine/Physics/PhysicsCollider2D.h"

#include "OverEngine/Scene/SceneCamera.h"

namespace OverEngine
{
	class Scene;

	enum class ComponentType
	{
		NameComponent, IDComponent, ActivationComponent, TransformComponent,
		CameraComponent, SpriteRendererComponent,
		PhysicsBody2DComponent, PhysicsColliders2DComponent
	};

	#define COMPONENT_TYPE(type) static ComponentType GetStaticType() { return ComponentType::type; }\
								virtual ComponentType GetComponentType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }\
								static const char* GetStaticName() { return #type; }

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

	struct NameComponent : public Component
	{
		String Name = String();

		NameComponent() = default;
		NameComponent(const NameComponent&) = default;
		NameComponent(Entity& entity, const String& name)
			: Component(entity), Name(name) {}

		COMPONENT_TYPE(NameComponent)
	};

	struct IDComponent : public Component
	{
		uint64_t ID = Random::UInt64();

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(Entity& entity, const uint64_t& id)
			: Component(entity), ID(id) {}

		COMPONENT_TYPE(IDComponent)
	};

	struct ActivationComponent : public Component
	{
		bool IsActive;

		ActivationComponent() = default;
		ActivationComponent(const ActivationComponent&) = default;
		ActivationComponent(Entity& entity, bool isActive)
			: Component(entity), IsActive(isActive) {}

		COMPONENT_TYPE(ActivationComponent)
	};

	////////////////////////////////////////////////////////
	// Renderer Components /////////////////////////////////
	////////////////////////////////////////////////////////

	struct CameraComponent : public Component
	{
		SceneCamera Camera;
		bool FixedAspectRatio = true;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		CameraComponent(Entity& entity, const SceneCamera& camera)
			: Component(entity), Camera(camera) {}

		CameraComponent(Entity& entity)
			: Component(entity) {}

		static SerializationContext* Reflect();

		COMPONENT_TYPE(CameraComponent)
	};

	struct SpriteRendererComponent : public Component
	{
		Ref<Texture2D> Sprite;

		Color Tint = Color(1.0f);

		Vector2 Tiling = Vector2(1.0f);
		Vector2 Offset = Vector2(0.0f);
		Vec2T<bool> Flip{ false, false };
		
		Vec2T<TextureWrapping> Wrapping{ TextureWrapping::None, TextureWrapping::None };
		TextureFiltering Filtering = TextureFiltering::None;

		float AlphaClipThreshold = 0.0f;

		/**
		 * first : Is overriding TextureBorderColor?
		 * second : Override TextureBorderColor to what?
		 */
		std::pair<bool, Color> TextureBorderColor{ false, Color(0.0f) };

	public:
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		SpriteRendererComponent(Entity& entity, Ref<Texture2D> sprite = nullptr)
			: Component(entity), Sprite(sprite) {}

		SpriteRendererComponent(Entity& entity, Ref<Texture2D> sprite, const Color& tint)
			: Component(entity), Sprite(sprite), Tint(tint) {}

		static SerializationContext* Reflect();

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
