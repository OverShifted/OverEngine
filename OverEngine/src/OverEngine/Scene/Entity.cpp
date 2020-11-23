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
}
