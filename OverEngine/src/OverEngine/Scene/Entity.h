#pragma once

#include "OverEngine/Core/Core.h"

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

	class Scene;

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		inline Scene* GetScene() const { return m_Scene; }

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) const
		{
			OE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			PushIDToSceneComponentList(entt::type_info<T>::id());
			return GetSceneRegistry().emplace<T>(m_EntityHandle, *this, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		T& AddComponentDontPassEntity(Args&&... args) const
		{
			OE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			PushIDToSceneComponentList(entt::type_info<T>::id());
			return GetSceneRegistry().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent() const
		{
			OE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return GetSceneRegistry().get<T>(m_EntityHandle);
		}

		template<typename T>
		ComponentRef<T> GetComponentRef() const
		{
			OE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return ComponentRef<T>(*this);
		}

		template<typename T>
		bool HasComponent() const
		{
			return GetSceneRegistry().has<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent() const
		{
			OE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			GetSceneRegistry().remove<T>(m_EntityHandle);

			RemoveIDFromSceneComponentList(entt::type_info<T>::id());
		}

		const Vector<entt::id_type>& GetComponentsTypeIDList() const;
		Vector<entt::id_type>& GetComponentsTypeIDList();

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
		entt::registry& GetSceneRegistry() const;
		void PushIDToSceneComponentList(const entt::id_type id) const;
		void RemoveIDFromSceneComponentList(const entt::id_type id) const;

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
