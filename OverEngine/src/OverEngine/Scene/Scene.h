#pragma once

#include "OverEngine/Core/Time/TimeStep.h"
#include "OverEngine/Physics/PhysicsWorld2D.h"

#include <entt.hpp>

namespace OverEngine
{
	class Entity;

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

		void OnUpdate(TimeStep deltaTime, Vector2 renderSurface);

		void OnPhysicsUpdate(TimeStep DeltaTime);
		void OnTransformUpdate();
		void OnRender(Vector2 renderSurface);

		Entity CreateEntity(const String& name = String());
		Entity CreateEntity(Entity& parent, const String& name = String());

		inline PhysicsWorld2D& GetPhysicsWorld2D() { return m_PhysicsWorld2D; }
		inline const PhysicsWorld2D& GetPhysicsWorld2D() const { return m_PhysicsWorld2D; }

		inline const Vector<Entity>& GetRootEntities() const { return m_RootEntities; }
		inline Vector<Entity>& GetRootEntities() { return m_RootEntities; }

		inline const Vector<Entity>& GetEntities() const { return m_Entities; }
		inline Vector<Entity>& GetEntities() { return m_Entities; }
	private:
		entt::registry m_Registry;
		PhysicsWorld2D m_PhysicsWorld2D;

		/* 
		 * To hold entities with nullptr parent.
		 * Useful for drawing graphs / trees.
		 */

		Vector<Entity> m_RootEntities;

		Vector<Entity> m_Entities;

		UnorderedMap<entt::entity, Vector<uint32_t>> m_EntitiesComponentsTypeIDList;

		friend class Entity;
	};
}