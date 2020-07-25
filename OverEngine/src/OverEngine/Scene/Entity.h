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
		Entity(const Entity & other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args &&... args)
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

		operator bool() const { return (uint32_t)m_EntityHandle != 0; }
	private:
		entt::entity m_EntityHandle;
		Scene* m_Scene;
	};
}