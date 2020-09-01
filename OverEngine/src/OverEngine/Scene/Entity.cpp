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
		auto& family = GetComponent<FamilyComponent>();

		if (!family.Parent)
			return;

		auto& lastParentChildren = family.Parent.GetComponent<FamilyComponent>().Children;
		auto it = std::find(lastParentChildren.begin(), lastParentChildren.end(), *this);
		OE_CORE_ASSERT(it != lastParentChildren.end(), "Entity is not in it's parent's child entities list!");
		lastParentChildren.erase(it);

		OE_CORE_INFO("Moving Entity #{} to the root. Last Parent : Entity #{}", GetID(), family.Parent.GetID());

		family.Parent = Entity(); // Null entity
		m_Scene->GetRootEntities().push_back(*this);
	}

	void Entity::SetParent(Entity parent)
	{
		auto& parentFamily = parent.GetComponent<FamilyComponent>();
		auto& family = GetComponent<FamilyComponent>();

		if (parent == family.Parent)
			return;

		parentFamily.Children.push_back(*this);

		if (family.Parent)
		{
			auto& lastParentChildren = family.Parent.GetComponent<FamilyComponent>().Children;

			auto it = std::find(lastParentChildren.begin(), lastParentChildren.begin(), *this);
			OE_CORE_ASSERT(it != lastParentChildren.end(), "Entity is not in it's parent's child entities list!");
			lastParentChildren.erase(it);
		}
		else
		{
			auto it = std::find(m_Scene->GetRootEntities().begin(), m_Scene->GetRootEntities().end(), *this);
			OE_CORE_ASSERT(it != m_Scene->GetRootEntities().end(), "Entity is not in the Scene's root entities!")
			m_Scene->GetRootEntities().erase(it);
		}

		family.Parent = parent;
	}

	void Entity::Destroy()
	{
		auto& family = GetComponent<FamilyComponent>();

		for (auto& child : family.Children)
			child.Destroy();

		if (!family.Parent)
		{
			auto it = std::find(m_Scene->m_RootEntities.begin(), m_Scene->m_RootEntities.end(), *this);
			OE_CORE_ASSERT(it != m_Scene->m_RootEntities.end(), "Entity is not in the Scene's root entities!");
			m_Scene->m_RootEntities.erase(it);
		}
		else
		{
			auto parentFamily = family.Parent.GetComponent<FamilyComponent>();
			auto it = std::find(parentFamily.Children.begin(), parentFamily.Children.end(), *this);
			OE_CORE_ASSERT(it != parentFamily.Children.end(), "Entity is not in it's parent's child entities!");
			parentFamily.Children.erase(it);
		}

		auto it = std::find(m_Scene->m_Entities.begin(), m_Scene->m_Entities.end(), *this);
		OE_CORE_ASSERT(it != m_Scene->m_Entities.end(), "Entity is not in the Scene's entities!");
		m_Scene->m_Entities.erase(it);

		m_Scene->m_Registry.destroy(m_EntityHandle);
	}
}
