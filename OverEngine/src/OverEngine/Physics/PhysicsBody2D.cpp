#include "pcheader.h"
#include "PhysicsBody2D.h"

#include <box2d/box2d.h>

namespace OverEngine
{
	PhysicsBody2D::PhysicsBody2D(PhysicsWorld2D& world, const PhysicsBodyProps& props)
	{
		b2BodyDef def;

		if (props.Type == PhysicsBodyType::Static)
			def.type = b2_staticBody;
		else if (props.Type == PhysicsBodyType::Dynamic)
			def.type = b2_dynamicBody;
		else if (props.Type == PhysicsBodyType::Kinematic)
			def.type = b2_kinematicBody;

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

		m_BodyHandle = world.m_WorldHandle.CreateBody(&def);
	}

	Vector2 PhysicsBody2D::GetPosition()
	{
		return { m_BodyHandle->GetPosition().x, m_BodyHandle->GetPosition().y };
	}

	float PhysicsBody2D::GetRotation()
	{
		return m_BodyHandle->GetAngle();
	}

	void PhysicsBody2D::SetPosition(const Vector2& position)
	{
		m_BodyHandle->SetTransform({ position.x, position.y }, m_BodyHandle->GetAngle());
	}

	void PhysicsBody2D::SetRotation(float rotation)
	{
		m_BodyHandle->SetTransform(m_BodyHandle->GetPosition(), rotation);
	}

	void PhysicsBody2D::AddCollider(const Ref<PhysicsCollider2D>& collider)
	{
		b2FixtureDef fixtureDef;

		fixtureDef.shape       = collider->GetShape()->m_ShapeHandle;
		fixtureDef.friction    = collider->GetMaterial().Friction;
		fixtureDef.restitution = collider->GetMaterial().Bounciness;
		fixtureDef.density     = collider->GetMaterial().Density;
		fixtureDef.isSensor    = collider->IsTrigger();

		m_ColliderToFixture[collider] = m_BodyHandle->CreateFixture(&fixtureDef);
	}

	void PhysicsBody2D::RemoveCollider(const Ref<PhysicsCollider2D>& collider)
	{
		m_BodyHandle->DestroyFixture(m_ColliderToFixture[collider]);
		m_ColliderToFixture.erase(collider);
	}

}