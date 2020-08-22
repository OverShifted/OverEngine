#include "pcheader.h"
#include "PhysicsWorld2D.h"

#include <box2d/box2d.h>

namespace OverEngine
{
	PhysicsWorld2D::PhysicsWorld2D(Vector2 gravity)
		: m_WorldHandle(b2Vec2(gravity.x, gravity.y))
	{
	}

	void PhysicsWorld2D::OnUpdate(TimeStep ts, uint32_t velocityIterations, uint32_t positionIterations)
	{
		m_WorldHandle.Step(ts, velocityIterations, positionIterations);
	}

	Vector2 PhysicsWorld2D::GetGravity() const
	{
		return { m_WorldHandle.GetGravity().x, m_WorldHandle.GetGravity().y };
	}

	void PhysicsWorld2D::SetGravity(const Vector2& gravity)
	{
		m_WorldHandle.SetGravity({ gravity.x, gravity.y });
	}
}