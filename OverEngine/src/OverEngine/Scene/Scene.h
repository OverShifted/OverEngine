#pragma once

#include "OverEngine/Core/Time/TimeStep.h"
#include "OverEngine/Physics/PhysicsWorld2D.h"

#include <entt.hpp>

namespace OverEngine
{
	class SceneCamera;
	class Entity;
	class Scene;

	Ref<Scene> CreateSceneOnDisk(const String& path);

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

		void OnUpdate(TimeStep deltaTime, Vector2 renderSurface);

		void OnPhysicsUpdate(TimeStep DeltaTime);

		bool OnRender(Vector2 renderSurface);
		void OnRender(const SceneCamera& camera, const Mat4x4& cameraTransform);

		inline PhysicsWorld2D& GetPhysicsWorld2D() { return m_PhysicsWorld2D; }
		inline const PhysicsWorld2D& GetPhysicsWorld2D() const { return m_PhysicsWorld2D; }

		inline const Vector<entt::entity>& GetRootHandles() const { return m_RootHandles; }
		inline Vector<entt::entity>& GetRootHandles() { return m_RootHandles; }

		inline uint32_t GetEntityCount() const;

		void Dump(const String& filePath);
		static Ref<Scene> LoadFile(const String& filePath);
	private:
		entt::registry m_Registry;
		PhysicsWorld2D m_PhysicsWorld2D;

		/**
		 * To hold entities with nullptr parent.
		 * Useful for drawing graphs / trees.
		 */

		Vector<entt::entity> m_RootHandles;

		UnorderedMap<entt::entity, Vector<uint32_t>> m_EntitiesComponentsTypeIDList;

		friend class Entity;
	};
}
