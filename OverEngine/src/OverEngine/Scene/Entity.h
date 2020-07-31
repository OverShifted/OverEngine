#pragma once

#include "Scene.h"

#include <entt.hpp>

namespace OverEngine
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene * scene);
		Entity(const Entity& other) = default;

		inline Scene* GetScene() const { return m_Scene; }

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			OE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, *this, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		T& AddComponentDontPassEntity(Args&&... args)
		{
			OE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			OE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			OE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		void SetParent(Entity& parent);

		operator bool() const { return m_EntityHandle != entt::null; }
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene;
	};
}