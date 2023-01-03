#pragma once

#include "Entity.h"
#include "Scene.h"

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/Random.h"

#include "OverEngine/Renderer/Texture.h"

#include "OverEngine/Physics/RigidBody2D.h"
#include "OverEngine/Physics/Collider2D.h"

#include "OverEngine/Scene/SceneCamera.h"
#include "OverEngine/Scene/ScriptableEntity.h"

namespace OverEngine
{
	struct Component
	{
		Entity AttachedEntity;

		Component(const Entity& attachedEntity)
			: AttachedEntity(attachedEntity) {}
	};

	////////////////////////////////////////////////////////
	/// Common Components //////////////////////////////////
	////////////////////////////////////////////////////////

	struct NameComponent : public Component
	{
		String Name;

		NameComponent(const NameComponent&) = default;
		NameComponent(const Entity& entity, const String& name)
			: Component(entity), Name(name) {}
	};

	struct IDComponent : public Component
	{
		uint64_t ID = Random::UInt64();

		IDComponent(const IDComponent&) = default;
		IDComponent(const Entity& entity, const uint64_t& id)
			: Component(entity), ID(id) {}
	};

	////////////////////////////////////////////////////////
	/// Renderer Components ////////////////////////////////
	////////////////////////////////////////////////////////

	struct CameraComponent : public Component
	{
		SceneCamera Camera;
		bool FixedAspectRatio = false;

		CameraComponent(const CameraComponent&) = default;

		CameraComponent(const Entity& entity, const SceneCamera& camera)
			: Component(entity), Camera(camera) {}

		CameraComponent(const Entity& entity)
			: Component(entity) {}
	};

	struct SpriteRendererComponent : public Component
	{
		Color Tint = Color(1.0f);

		Ref<Texture2D> Sprite;

		Vector2 Tiling = Vector2(1.0f);
		Vector2 Offset = Vector2(0.0f);
		TextureFlip Flip = TextureFlip_None;

		// Useful for SubTextures
		bool ForceTile = false;

		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		SpriteRendererComponent(const Entity& entity, Ref<Texture2D> sprite = nullptr)
			: Component(entity), Sprite(sprite) {}

		SpriteRendererComponent(const Entity& entity, Ref<Texture2D> sprite, const Color& tint)
			: Component(entity), Sprite(sprite), Tint(tint) {}
	};

	////////////////////////////////////////////////////////
	/// Physics Components /////////////////////////////////
	////////////////////////////////////////////////////////

	struct RigidBody2DComponent : public Component
	{
		Ref<RigidBody2D> RigidBody = nullptr;

		RigidBody2DComponent(const RigidBody2DComponent& other)
			: Component(other.AttachedEntity), RigidBody(other.RigidBody) {}

		RigidBody2DComponent(RigidBody2DComponent&& other)
			: Component(other.AttachedEntity)
		{
			if (other.RigidBody && !other.RigidBody->IsDeployed())
				RigidBody = other.RigidBody;
		}

		void operator=(RigidBody2DComponent&& other)
		{
			AttachedEntity = other.AttachedEntity;
			if (other.RigidBody && !other.RigidBody->IsDeployed())
				RigidBody = other.RigidBody;
		}

		void operator=(const RigidBody2DComponent& other)
		{
			AttachedEntity = other.AttachedEntity;
			RigidBody = RigidBody2D::Create(other.RigidBody->GetProps());
		}

		RigidBody2DComponent(const Entity& entity, const RigidBody2DProps& props = RigidBody2DProps())
			: Component(entity), RigidBody(RigidBody2D::Create(props)) {}

		~RigidBody2DComponent()
		{
			if (RigidBody && RigidBody->IsDeployed())
				RigidBody->UnDeploy();
		}
	};

	// Store's all colliders attached to an Entity
	struct Collider2DComponent : public Component
	{
		Ref<Collider2D> Collider;

		Collider2DComponent(const Collider2DComponent& other)
			: Component(other.AttachedEntity)
		{
			Collider = Collider2D::Create(other.Collider->GetProps());
		}

		Collider2DComponent(const Entity& entity)
			: Component(entity) {}
	};

	////////////////////////////////////////////////////////
	/// Script Components //////////////////////////////////
	////////////////////////////////////////////////////////

	struct NativeScriptsComponent : public Component
	{
		struct ScriptData
		{
			ScriptableEntity* Instance = nullptr;

			std::function<ScriptableEntity*()> InstantiateScript;
			void (*DestroyScript)(ScriptData*);

			~ScriptData()
			{
				if (Instance)
					DestroyScript(this);
			}
		};

		bool Runtime = false;
		UnorderedMap<size_t, ScriptData> Scripts;

		// Don't copy 'Scripts' to other instance when `Runtime` is true; since it will lead to double free on entt::registry::destroy
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
				OE_CORE_ASSERT(false, "Script of `typeid().hash_code() = {}` is already attached to Entity {0:x} (named `{}`)!", hash, AttachedEntity.GetComponent<IDComponent>().ID, AttachedEntity.GetComponent<NameComponent>().Name);
				return;
			}

			Scripts[hash] = ScriptData{
				nullptr,

				// TODO: Use C++20 features here https://stackoverflow.com/a/49902823/11814750
				[args = std::make_tuple(std::forward<Args>(args)...)]() mutable
				{
					return std::apply([](auto&& ... args) {
						return static_cast<ScriptableEntity*>(new T(std::forward<Args>(args)...));
					}, std::move(args));
				},

				[](ScriptData* s) { delete s->Instance; s->Instance = nullptr; }
			};

			if (Runtime)
			{
				auto& script = Scripts.at(hash);
				script.Instance = script.InstantiateScript();
			}
		}

		template<typename T>
		inline T& GetScript() { return *((T*)Scripts[typeid(T).hash_code()].Instance); }

		template<typename T>
		inline bool HasScript() { return HasScript(typeid(T).hash_code()); }

		template<typename T>
		inline void RemoveScript() { RemoveScript(typeid(T).hash_code()); }

		inline bool HasScript(size_t hash) const { return Scripts.count(hash); }
		void RemoveScript(size_t hash);
	};

	class WrenScriptInstance;
	struct ScriptComponent : public Component
	{
		Ref<WrenScriptInstance> Script;
		
		ScriptComponent(const Entity& entity)
			: Component(entity) {}

		ScriptComponent(const Entity& entity, const Ref<WrenScriptInstance>& script)
			: Component(entity), Script(script) {}

		ScriptComponent(const ScriptComponent& other)
			: Component(other.AttachedEntity)
		{
			Script = other.Script;
		}
	};
}
