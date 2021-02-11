#include "pcheader.h"
#include "Entity.h"

#include "OverEngine/Scene/Components.h"
#include "OverEngine/Scene/TransformComponent.h"

namespace OverEngine
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

	void Entity::Destroy()
	{
		// TODO: Move to transform destructor

		auto& transform = GetComponent<TransformComponent>();

		transform.DetachFromParent();

		while (transform.GetChildrenHandles().size() > 0)
			Entity{ transform.GetChildrenHandles()[0], m_Scene }.Destroy();

		auto it = STD_CONTAINER_FIND(m_Scene->m_RootHandles, m_EntityHandle);
		OE_CORE_ASSERT(it != m_Scene->m_RootHandles.end(), "Entity is not in the Scene's root entities!");
		m_Scene->m_RootHandles.erase(it);

		m_Scene->m_Registry.destroy(m_EntityHandle);
	}

	entt::registry& Entity::GetSceneRegistry() const
	{
		return m_Scene->m_Registry;
	}

	void Entity::PushIDToSceneComponentList(const entt::id_type id) const
	{
		m_Scene->m_ComponentList[m_EntityHandle].push_back(id);
	}

	void Entity::RemoveIDFromSceneComponentList(const entt::id_type id) const
	{
		auto& componentList = m_Scene->m_ComponentList[m_EntityHandle];
		auto it = STD_CONTAINER_FIND(componentList, id);
		if (it != componentList.end())
			componentList.erase(it);
	}

	const Vector<entt::id_type>& Entity::GetComponentsTypeIDList() const
	{
		return m_Scene->m_ComponentList[m_EntityHandle];
	}

	Vector<entt::id_type>& Entity::GetComponentsTypeIDList()
	{
		return m_Scene->m_ComponentList[m_EntityHandle];
	}
}
