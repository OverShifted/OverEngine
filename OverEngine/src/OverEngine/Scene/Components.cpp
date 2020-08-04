#include "pcheader.h"
#include "Components.h"

namespace OverEngine
{
	PhysicsBodyComponent::PhysicsBodyComponent(Entity& entity, const PhysicsBodyProps& props)
		: Body(CreateRef<PhysicsBody2D>(entity.GetScene()->GetPhysicsWorld2D(), props))
	{
		if (entity.HasComponent<PhysicsColliders2DComponent>())
		{
			auto& colliders = entity.GetComponent<PhysicsColliders2DComponent>();
			colliders.AttachedBody = Body;

			for (const auto& collider : colliders.Colliders)
			{
				Body->AddCollider(collider);
			}
		}
	}
}