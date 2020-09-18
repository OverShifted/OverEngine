#include "pcheader.h"
#include "Entity.h"

#include "OverEngine/Scene/Components.h"

namespace OverEngine
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

	void Entity::ClearParent()
	{
		auto& base = GetComponent<BaseComponent>();

		if (!base.Parent)
			return;

		auto& lastParentChildren = base.Parent.GetComponent<BaseComponent>().Children;
		auto it = std::find(lastParentChildren.begin(), lastParentChildren.end(), *this);
		OE_CORE_ASSERT(it != lastParentChildren.end(), "Entity is not in it's parent's child entities list!");
		lastParentChildren.erase(it);

		OE_CORE_INFO("Moving Entity #{} to the root. Last Parent : Entity #{}", GetRuntimeID(), base.Parent.GetRuntimeID());

		base.Parent = Entity(); // Null entity
		m_Scene->GetRootEntities().push_back(*this);
	}

	void Entity::SetParent(Entity parent)
	{
		auto& base = GetComponent<BaseComponent>();

		if (parent == base.Parent)
			return;

		parent.GetComponent<BaseComponent>().Children.push_back(*this);

		if (base.Parent)
		{
			auto& lastParentChildren = base.Parent.GetComponent<BaseComponent>().Children;

			auto it = std::find(lastParentChildren.begin(), lastParentChildren.end(), *this);
			OE_CORE_ASSERT(it != lastParentChildren.end(), "Entity is not in it's parent's child entities list!");
			lastParentChildren.erase(it);
		}
		else
		{
			auto it = std::find(m_Scene->GetRootEntities().begin(), m_Scene->GetRootEntities().end(), *this);
			OE_CORE_ASSERT(it != m_Scene->GetRootEntities().end(), "Entity is not in the Scene's root entities!")
			m_Scene->GetRootEntities().erase(it);
		}

		base.Parent = parent;
	}

	void Entity::Destroy()
	{
		auto& base = GetComponent<BaseComponent>();

		while (base.Children.size() > 0)
			Entity(base.Children[0]).Destroy(); // Copy to prevent self-editing

		if (!base.Parent)
		{
			auto it = std::find(m_Scene->m_RootEntities.begin(), m_Scene->m_RootEntities.end(), *this);
			OE_CORE_ASSERT(it != m_Scene->m_RootEntities.end(), "Entity is not in the Scene's root entities!");
			m_Scene->m_RootEntities.erase(it);
		}
		else
		{
			auto& parentChildren = base.Parent.GetComponent<BaseComponent>().Children;
			auto it = std::find(parentChildren.begin(), parentChildren.end(), *this);
			OE_CORE_ASSERT(it != parentChildren.end(), "Entity is not in it's parent's child entities!");
			parentChildren.erase(it);
		}

		m_Scene->m_Registry.destroy(m_EntityHandle);
	}
}
