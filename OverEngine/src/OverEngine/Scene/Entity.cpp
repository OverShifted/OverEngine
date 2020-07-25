#include "pcheader.h"
#include "Entity.h"

namespace OverEngine
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
}