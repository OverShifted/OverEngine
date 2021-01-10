#pragma once

#include "OverEngine/Core/Time/TimeStep.h"
#include "OverEngine/Core/Random.h"
#include "OverEngine/Physics/PhysicWorld2D.h"
#include "OverEngine/Scripting/LuaScriptingEngine.h"
#include "OverEngine/Assets/AssetCollection.h"

#include <entt.hpp>

namespace OverEngine
{
	class SceneCamera;
	class Entity;
	class Scene;

	struct Physics2DSettings
	{
		Vector2 gravity = Vector2(0.0f, -9.8f);
	};

	struct SceneSettings
	{
		Physics2DSettings physics2DSettings;
	};

	class SceneSerializer;

	class Scene
	{
	public:
		Scene(const SceneSettings& settings = SceneSettings());
		Scene(Scene& other);
		~Scene();

		Entity CreateEntity(const String& name = String(), uint64_t uuid = Random::UInt64());
		Entity CreateEntity(Entity& parent, const String& name = String(), uint64_t uuid = Random::UInt64());

		/**
		 * Func should be void(*func)(Entity);
		 * Using template allow func to be a capturing lambda
		 */
		template <typename Func>
		void Each(Func func)
		{
			m_Registry.each([&](auto entityID)
			{
				Entity entity(entityID, this);
				func(entity);
			});
		}

		void OnUpdate(TimeStep deltaTime);
		void OnPhysicsUpdate(TimeStep deltaTime);
		void OnScriptsUpdate(TimeStep deltaTime);

		void OnScenePlay();
		void InitializePhysics();
		void InitializeScripts();

		// Rendering
		bool OnRender();
		void RenderSprites();
		void SetViewportSize(uint32_t width, uint32_t height);

		void LoadReferences(AssetCollection& assetCollection);

		inline PhysicWorld2D& GetPhysicWorld2D() { return *m_PhysicWorld2D; }
		inline const PhysicWorld2D& GetPhysicWorld2D() const { return *m_PhysicWorld2D; }

		inline const Vector<entt::entity>& GetRootHandles() const { return m_RootHandles; }
		inline Vector<entt::entity>& GetRootHandles() { return m_RootHandles; }

		inline uint32_t GetEntityCount() const;

		inline bool Exists(const entt::entity& entity) { return m_Registry.valid(entity); }
		
		void OnCollisionEnter(const Collision2D& collision);
		void OnCollisionExit(const Collision2D& collision);
	private:
		entt::registry m_Registry;
		PhysicWorld2D* m_PhysicWorld2D = nullptr;
		LuaScriptingEngine* m_LuaEngine = nullptr;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0; // TODO: set viewport size for new camera components

		/**
		 * To hold entities with entt::null parent.
		 * Useful for drawing graphs / trees.
		 */
		Vector<entt::entity> m_RootHandles;
		UnorderedMap<entt::entity, Vector<entt::id_type>> m_ComponentList;

		friend class Entity;
		friend class SceneSerializer;
	};
}
