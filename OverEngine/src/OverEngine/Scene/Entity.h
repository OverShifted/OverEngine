#pragma once

#include "Scene.h"

#include <entt.hpp>

namespace OverEngine
{
	template<typename T>
	uint32_t GetComponentTypeID()
	{
		return entt::type_info<T>::id();
	}

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
			m_Scene->m_EntitiesComponentsTypeIDList[m_EntityHandle].push_back(entt::type_info<T>::id());
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, *this, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		T& AddComponentDontPassEntity(Args&&... args)
		{
			OE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			m_Scene->m_EntitiesComponentsTypeIDList[m_EntityHandle].push_back(entt::type_info<T>::id());
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
			
			auto& componentList = m_Scene->m_EntitiesComponentsTypeIDList[m_EntityHandle];
			auto it = std::find(componentList.begin(), componentList.end(), entt::type_info<T>::id());
			if (it != componentList.end())
				componentList.erase(it);
		}

		const Vector<uint32_t>& GetEntitiesComponentsTypeIDList() const { return m_Scene->m_EntitiesComponentsTypeIDList[m_EntityHandle]; }
		Vector<uint32_t>& GetEntitiesComponentsTypeIDList() { return m_Scene->m_EntitiesComponentsTypeIDList[m_EntityHandle]; }

		void SetParent(Entity parent);
		void Destroy();

		inline uint32_t GetID() { OE_CORE_ASSERT(m_EntityHandle != entt::null, "Entity handle is null!"); return (uint32_t)m_EntityHandle; }

		operator bool() const { return m_EntityHandle != entt::null; }
		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle; }
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene;
	};
}