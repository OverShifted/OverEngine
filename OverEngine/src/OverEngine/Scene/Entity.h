#pragma once

#include "OverEngine/Core/Core.h"
#include "Scene.h"

#include <entt.hpp>

namespace OverEngine
{
	template<typename T>
	struct ComponentRef;

	template<typename T>
	entt::id_type GetComponentTypeID()
	{
		return entt::type_info<T>::id();
	}

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		inline Scene* GetScene() const { return m_Scene; }

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			OE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			m_Scene->m_ComponentList[m_EntityHandle].push_back(entt::type_info<T>::id());
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, *this, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		T& AddComponentDontPassEntity(Args&&... args)
		{
			OE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			m_Scene->m_ComponentList[m_EntityHandle].push_back(entt::type_info<T>::id());
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			OE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		ComponentRef<T> GetComponentRef()
		{
			OE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return ComponentRef<T>(*this);
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

			auto& componentList = m_Scene->m_ComponentList[m_EntityHandle];
			auto it = STD_CONTAINER_FIND(componentList, entt::type_info<T>::id());
			if (it != componentList.end())
				componentList.erase(it);
		}

		const Vector<entt::id_type>& GetComponentsTypeIDList() const { return m_Scene->m_ComponentList[m_EntityHandle]; }
		Vector<entt::id_type>& GetComponentsTypeIDList() { return m_Scene->m_ComponentList[m_EntityHandle]; }

		void Destroy();

		inline entt::entity GetRuntimeID() const { OE_CORE_ASSERT(m_EntityHandle != entt::null, "Entity handle is null!"); return m_EntityHandle; }

		operator bool() const { return m_EntityHandle != entt::null; }
		operator uint32_t() const { return (uint32_t)GetRuntimeID(); }
		operator entt::entity() const { return GetRuntimeID(); }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}
	
		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene;
	};

	template<typename T>
	struct ComponentRef
	{
		ComponentRef(Entity entity) : AttachedEntity(entity) {}
		Entity AttachedEntity;

		operator T& () { return AttachedEntity.GetComponent<T>(); }
		T* operator ->() { return &AttachedEntity.GetComponent<T>(); }
		T& operator *() { return AttachedEntity.GetComponent<T>(); }
	};
}
