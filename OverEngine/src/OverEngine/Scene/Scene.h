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

		void OnUpdate(TimeStep DeltaTime);

		Entity CreateEntity(const String& name = String());
		Entity CreateEntity(Entity& parent, const String& name = String());

		inline PhysicsWorld2D& GetPhysicsWorld2D() { return m_PhysicsWorld2D; }

		inline const Vector<Entity>& GetRootEntities() const { return m_RootEntities; }
		inline Vector<Entity>& GetRootEntities() { return m_RootEntities; }
	private:
		void UpdatePhysicsAndTransform(TimeStep DeltaTime);
		void Render();
	private:
		entt::registry m_Registry;
		PhysicsWorld2D m_PhysicsWorld2D;

		Vector<Entity> m_RootEntities;

		friend class Entity;
	};
}