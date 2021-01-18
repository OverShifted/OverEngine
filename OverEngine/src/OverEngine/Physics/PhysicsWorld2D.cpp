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

	Ref<RigidBody2D> PhysicsWorld2D::CreateRigidBody(const RigidBody2DProps& props)
	{
		b2BodyDef def;

		def.type = (b2BodyType)props.Type;

		def.position.Set(props.Position.x, props.Position.y);
		def.angle = props.Rotation;

		def.linearVelocity.Set(props.LinearVelocity.x, props.LinearVelocity.y);
		def.angularVelocity = props.AngularVelocity;

		def.linearDamping = props.LinearDamping;
		def.angularVelocity = props.AngularDamping;

		def.allowSleep = props.AllowSleep;
		def.awake = props.Awake;
		def.enabled = props.Enabled;

		def.fixedRotation = props.FixedRotation;
		def.gravityScale = props.GravityScale;
		def.bullet = props.Bullet;

		auto body = CreateRef<RigidBody2D>(m_WorldHandle.CreateBody(&def));
		m_Bodies.push_back(body);
		return body;
	}

	void PhysicsWorld2D::DestroyRigidBody(const Ref<RigidBody2D>& rigidBody)
	{
		if (!rigidBody || !rigidBody->m_BodyHandle)
			return;

		auto it = STD_CONTAINER_FIND(m_Bodies, rigidBody);

		if (it == m_Bodies.end())
			return;

		m_WorldHandle.DestroyBody(rigidBody->m_BodyHandle);
		rigidBody->m_BodyHandle = nullptr;
		m_Bodies.erase(it);
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

	void PhysicsWorld2D::CollisionListener::BeginContact(b2Contact* contact)
	{
		Collider2D* colliderA_ptr = (Collider2D*)(contact->GetFixtureA()->GetUserData().pointer);
		Collider2D* colliderB_ptr = (Collider2D*)(contact->GetFixtureB()->GetUserData().pointer);

		if (!colliderA_ptr || !colliderB_ptr)
			return;
		
		auto colliderA = colliderA_ptr->GetAttachedBody()->FindCollider(colliderA_ptr);
		auto colliderB = colliderB_ptr->GetAttachedBody()->FindCollider(colliderB_ptr);

		Collision2D collision;
		collision.ColliderA = colliderA;
		collision.ColliderB = colliderB;

		OnCollisionEnter(collision, UserData);
	}

	void PhysicsWorld2D::CollisionListener::EndContact(b2Contact* contact)
	{
		Collider2D* colliderA_ptr = (Collider2D*)(contact->GetFixtureA()->GetUserData().pointer);
		Collider2D* colliderB_ptr = (Collider2D*)(contact->GetFixtureB()->GetUserData().pointer);

		if (!colliderA_ptr || !colliderB_ptr)
			return;
		
		auto colliderA = colliderA_ptr->GetAttachedBody()->FindCollider(colliderA_ptr);
		auto colliderB = colliderB_ptr->GetAttachedBody()->FindCollider(colliderB_ptr);

		Collision2D collision;
		collision.ColliderA = colliderA;
		collision.ColliderB = colliderB;

		OnCollisionExit(collision, UserData);
	}
}
