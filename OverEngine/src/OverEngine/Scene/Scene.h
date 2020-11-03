#pragma once

#include "OverEngine/Core/Time/TimeStep.h"
#include "OverEngine/Core/Random.h"
#include "OverEngine/Physics/PhysicsWorld2D.h"

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

		void OnPhysicsUpdate(TimeStep DeltaTime);

		// Rendering
		bool OnRender();
		void RenderSprites();
		void SetViewportSize(uint32_t width, uint32_t height);

		inline PhysicsWorld2D& GetPhysicsWorld2D() { return m_PhysicsWorld2D; }
		inline const PhysicsWorld2D& GetPhysicsWorld2D() const { return m_PhysicsWorld2D; }

		inline const Vector<entt::entity>& GetRootHandles() const { return m_RootHandles; }
		inline Vector<entt::entity>& GetRootHandles() { return m_RootHandles; }

		inline uint32_t GetEntityCount() const;
	private:
		entt::registry m_Registry;
		PhysicsWorld2D m_PhysicsWorld2D;

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
