#pragma once

#include "Entity.h"
#include "Scene.h"

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Serialization/Serializer.h"
#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/Random.h"

#include "OverEngine/Renderer/Texture.h"

#include "OverEngine/Physics/RigidBody2D.h"
#include "OverEngine/Physics/Collider2D.h"

#include "OverEngine/Scene/SceneCamera.h"
#include "OverEngine/Scene/ScriptableEntity.h"

namespace OverEngine
{
	class Scene;

	enum class ComponentType
	{
		NameComponent, IDComponent, ActivationComponent, TransformComponent,
		CameraComponent, SpriteRendererComponent,
		RigidBody2DComponent, Colliders2DComponent,
		NativeScriptsComponent, LuaScriptsComponent
	};

	#define COMPONENT_TYPE(type) static ComponentType GetStaticType() { return ComponentType::type; }\
								 virtual ComponentType GetComponentType() const override { return GetStaticType(); }\
								 virtual const char* GetName() const override { return #type; }\
								 static const char* GetStaticName() { return #type; }

	struct Component
	{
		Component(const Entity& attachedEntity, bool enabled = true)
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

		NameComponent(const NameComponent&) = default;
		NameComponent(const Entity& entity, const String& name)
			: Component(entity), Name(name) {}

		COMPONENT_TYPE(NameComponent)
	};

	struct IDComponent : public Component
	{
		uint64_t ID = Random::UInt64();

		IDComponent(const IDComponent&) = default;
		IDComponent(const Entity& entity, const uint64_t& id)
			: Component(entity), ID(id) {}

		COMPONENT_TYPE(IDComponent)
	};

	struct ActivationComponent : public Component
	{
		bool IsActive;

		ActivationComponent(const ActivationComponent&) = default;
		ActivationComponent(const Entity& entity, bool isActive)
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

		CameraComponent(const CameraComponent&) = default;

		CameraComponent(const Entity& entity, const SceneCamera& camera)
			: Component(entity), Camera(camera) {}

		CameraComponent(const Entity& entity)
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
		TextureFlip Flip = 0;
		
		Vec2T<TextureWrap> Wrap{ TextureWrap::None, TextureWrap::None };
		TextureFilter Filter = TextureFilter::None;

	public:
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		SpriteRendererComponent(const Entity& entity, Ref<Texture2D> sprite = nullptr)
			: Component(entity), Sprite(sprite) {}

		SpriteRendererComponent(const Entity& entity, Ref<Texture2D> sprite, const Color& tint)
			: Component(entity), Sprite(sprite), Tint(tint) {}

		static SerializationContext* Reflect();

		COMPONENT_TYPE(SpriteRendererComponent)
	};

	////////////////////////////////////////////////////////
	// Physics Components //////////////////////////////////
	////////////////////////////////////////////////////////

	struct RigidBody2DComponent : public Component
	{
		// Used as pre-runtime storage
		RigidBody2DProps Initializer;

		// Used for runtime
		Ref<RigidBody2D> RigidBody;

		RigidBody2DComponent(const RigidBody2DComponent&) = default;

		RigidBody2DComponent(const Entity& entity, const RigidBody2DProps& props = RigidBody2DProps())
			: Component(entity), Initializer(props) {}

		~RigidBody2DComponent()
		{
			AttachedEntity.GetScene()->GetPhysicsWorld2D().DestroyRigidBody(RigidBody);
		}

		static SerializationContext* Reflect();

		COMPONENT_TYPE(RigidBody2DComponent)
	};

	// Store's all colliders attached to an Entity
	struct Colliders2DComponent : public Component
	{
		struct ColliderData
		{
			Collider2DProps Initializer;
			Ref<Collider2D> Collider;
		};

		Vector<ColliderData> Colliders;

		Colliders2DComponent(const Colliders2DComponent&) = default;

		Colliders2DComponent(const Entity& entity)
			: Component(entity) {}

		COMPONENT_TYPE(Colliders2DComponent)
	};

	////////////////////////////////////////////////////////
	// Script Components ///////////////////////////////////
	////////////////////////////////////////////////////////

	struct NativeScriptsComponent : public Component
	{
		struct _Script
		{
			ScriptableEntity* Instance = nullptr;

			std::function<ScriptableEntity*()> InstantiateScript;
			void (*DestroyScript)(_Script*);

			~_Script()
			{
				if (Instance)
					DestroyScript(this);
			}
		};

		bool Runtime = false;
		UnorderedMap<size_t, _Script> Scripts;


		// Don't copy 'Scripts' to other instance when 'Runtime' is true; since it will lead to double free on entt::registry::destroy
		NativeScriptsComponent(const NativeScriptsComponent& other)
			: Component(other.AttachedEntity)
		{
			if (!other.Runtime)
				Scripts = other.Scripts;
		}

		NativeScriptsComponent(const Entity& entity)
			: Component(entity) {}

		template<typename T, typename... Args>
		void AddScript(Args&&... args)
		{
			auto hash = typeid(T).hash_code();

			if (HasScript(hash))
			{
				OE_CORE_ASSERT(false, "Script is already attached to Entity!");
				return;
			}

			Scripts[hash] = _Script{
				nullptr,

				// TODO: Use C++20 features here https://stackoverflow.com/a/49902823/11814750
				[args = std::make_tuple(std::forward<Args>(args)...)]() mutable
				{
					return std::apply([](auto&& ... args) {
						return static_cast<ScriptableEntity*>(new T(std::forward<Args>(args)...));
					}, std::move(args));
				},

				[](_Script* s) { delete s->Instance; s->Instance = nullptr; }
			};

			if (Runtime)
			{
				auto& script = Scripts[hash];
				script.Instance = script.InstantiateScript();
			}
		}

		template<typename T>
		T& GetScript()
		{
			return *((T*)Scripts[typeid(T).hash_code()].Instance);
		}

		template<typename T>
		bool HasScript()
		{
			return HasScript(typeid(T).hash_code());
		}

		bool HasScript(size_t hash)
		{
			return Scripts.count(hash);
		}

		template<typename T>
		void RemoveScript()
		{
			RemoveScript(typeid(T).hash_code());
		}

		void RemoveScript(size_t hash)
		{
			if (!HasScript(hash))
			{
				OE_CORE_ASSERT(false, "Script is not attached to Entity!");
				return;
			}

			if (Runtime)
			{
				auto& script = Scripts[hash];
				script.DestroyScript(&script);
			}

			Scripts.erase(hash);
		}

		COMPONENT_TYPE(NativeScriptsComponent)
	};

	struct LuaScriptsComponent : public Component
	{
		// TODO: Store compiled bytecode instead of raw string
		// name => source
		UnorderedMap<String, String> Scripts;

		LuaScriptsComponent(const LuaScriptsComponent&) = default;

		LuaScriptsComponent(const Entity& entity)
			: Component(entity) {}

		COMPONENT_TYPE(LuaScriptsComponent)
	};
}
