#include "pcheader.h"
#include "Entity.h"

#include "OverEngine/Scene/Components.h"

namespace OverEngine
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

	void Entity::SetParent(Entity& parent)
	{
		auto& parentFamily = parent.GetComponent<FamilyComponent>();
		parentFamily.Children.push_back(this);

		auto& family = GetComponent<FamilyComponent>();

		if (family.Parent)
		{
			auto& lastParentChildren = family.Parent->GetComponent<FamilyComponent>().Children;

			auto it = std::find(lastParentChildren.begin(), lastParentChildren.begin(), this);
			if (it != lastParentChildren.end())
				lastParentChildren.erase(it);
		}

		family.Parent = &parent;
	}
}