#include "pcheader.h"
#include "PhysicsWorld2D.h"

#include <box2d/box2d.h>

namespace OverEngine
{
	PhysicsWorld2D::PhysicsWorld2D(Vector2 gravity)
		: m_WorldHandle(b2Vec2(gravity.x, gravity.y))
	{
		m_WorldHandle.SetContactListener(static_cast<b2ContactListener*>(&m_CollisionListener));
	}

	Vector2 PhysicsWorld2D::GetGravity() const
	{
		return { m_WorldHandle.GetGravity().x, m_WorldHandle.GetGravity().y };
	}

	void PhysicsWorld2D::SetGravity(const Vector2& gravity)
	{
		m_WorldHandle.SetGravity({ gravity.x, gravity.y });
	}

	void PhysicsWorld2D::OnUpdate(TimeStep ts, uint32_t velocityIterations, uint32_t positionIterations)
	{
		m_WorldHandle.Step(ts, velocityIterations, positionIterations);
	}

	void PhysicsWorld2D::SetOnCollisionCallbackUserData(void* userData)
	{
		m_CollisionListener.UserData = userData;
	}

	void PhysicsWorld2D::SetOnCollisionEnterCallback(void (*callback)(const Collision2D&, void*))
	{
		m_CollisionListener.OnCollisionEnter = callback;
	}

	void PhysicsWorld2D::SetOnCollisionExitCallback(void (*callback)(const Collision2D&, void*))
	{
		m_CollisionListener.OnCollisionExit = callback;
	}

    PhysicsWorld2D::~PhysicsWorld2D()
    {
        for (const auto& body : m_Bodies)
        {
            body->m_WorldHandle = nullptr;
            body->m_BodyHandle = nullptr;
        }
    }

	void PhysicsWorld2D::CollisionListener::HandleContact(b2Contact* contact, void (*callback)(const Collision2D&, void*))
	{
		Collider2D* colliderA_ptr = reinterpret_cast<Collider2D*>(contact->GetFixtureA()->GetUserData().pointer);
		Collider2D* colliderB_ptr = reinterpret_cast<Collider2D*>(contact->GetFixtureB()->GetUserData().pointer);

		if (!(colliderA_ptr && colliderB_ptr))
			return;

		Collision2D collision;
		collision.ColliderA = colliderA_ptr->shared_from_this();
		collision.ColliderB = colliderB_ptr->shared_from_this();

		callback(collision, UserData);
	}

    void PhysicsWorld2D::CollisionListener::BeginContact(b2Contact* contact)
	{
		HandleContact(contact, OnCollisionEnter);
	}

	void PhysicsWorld2D::CollisionListener::EndContact(b2Contact* contact)
	{
		HandleContact(contact, OnCollisionExit);
	}
}
