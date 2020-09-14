#pragma once

#include "OverEngine/Core/Time/TimeStep.h"
#include "OverEngine/Physics/PhysicsWorld2D.h"

#include <entt.hpp>

namespace OverEngine
{
	class Entity;
	class Scene;

	Ref<Scene> CreateSceneOnDisk(const String& path);
	Ref<Scene> LoadSceneFromFile(const String& path);
	void SaveSceneToFile(const String& path, Ref<Scene> scene);

	struct Physics2DSettings
	{
		Vector2 gravity = Vector2(0.0f, -9.8f);
	};

	struct SceneSettings
	{
		Physics2DSettings physics2DSettings;
	};

	class Scene
	{
	public:
		Scene(const SceneSettings& settings = SceneSettings());
		~Scene();

		Entity CreateEntity(const String& name = String());
		Entity CreateEntity(Entity& parent, const String& name = String());

		/*
		 * Func should be void(*func)(Entity);
		 * Using template allow func (probably a lambda expression)
		 * to be able to use capture flags
		 */

		template <typename Func>
		void Each(Func func)
		{
			m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID , this };
				func(entity);
			});
		}

		void OnUpdate(TimeStep deltaTime, Vector2 renderSurface);

		void OnPhysicsUpdate(TimeStep DeltaTime);
		void OnTransformUpdate();
		bool OnRender(Vector2 renderSurface);

		inline PhysicsWorld2D& GetPhysicsWorld2D() { return m_PhysicsWorld2D; }
		inline const PhysicsWorld2D& GetPhysicsWorld2D() const { return m_PhysicsWorld2D; }

		inline const Vector<Entity>& GetRootEntities() const { return m_RootEntities; }
		inline Vector<Entity>& GetRootEntities() { return m_RootEntities; }

		inline uint32_t GetEntityCount();
	private:
		entt::registry m_Registry;
		PhysicsWorld2D m_PhysicsWorld2D;

		/* 
		 * To hold entities with nullptr parent.
		 * Useful for drawing graphs / trees.
		 */

		Vector<Entity> m_RootEntities;

		UnorderedMap<entt::entity, Vector<uint32_t>> m_EntitiesComponentsTypeIDList;

		friend class Entity;
	};
}
