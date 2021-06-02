#include "pcheader.h"
#include "RigidBody2D.h"

#include "PhysicsWorld2D.h"

#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_shape.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

namespace OverEngine
{
	Ref<RigidBody2D> RigidBody2D::Create(const RigidBody2DProps& props)
	{
		return CreateRef<RigidBody2D>(props);
	}

	RigidBody2D::RigidBody2D(const RigidBody2DProps& props)
		: m_Props(props)
	{
	}

	RigidBody2D::~RigidBody2D()
	{
		if (m_BodyHandle && m_WorldHandle)
			UnDeploy(true);
	}

	void RigidBody2D::Deploy(PhysicsWorld2D* world)
	{
		b2BodyDef def;

		def.type = (b2BodyType)((int)m_Props.Type - 1);

		def.position.Set(m_Props.Dynamics.Position.x, m_Props.Dynamics.Position.y);
		def.angle = m_Props.Dynamics.Rotation;

		def.linearVelocity.Set(m_Props.Dynamics.LinearVelocity.x, m_Props.Dynamics.LinearVelocity.y);
		def.angularVelocity = m_Props.Dynamics.AngularVelocity;

		def.linearDamping = m_Props.LinearDamping;
		def.angularVelocity = m_Props.AngularDamping;

		def.allowSleep = m_Props.AllowSleep;
		def.awake = m_Props.Dynamics.IsAwake;
		def.enabled = m_Props.Enabled;

		def.fixedRotation = m_Props.FixedRotation;
		def.gravityScale = m_Props.GravityScale;
		def.bullet = m_Props.Bullet;

		world->m_Bodies.push_back(shared_from_this());
		m_WorldHandle = world;
		m_BodyHandle = world->m_WorldHandle.CreateBody(&def);
	}

	void RigidBody2D::UnDeploy(bool onDestruct)
	{
	    if (!onDestruct)
		    m_WorldHandle->m_Bodies.erase(STD_CONTAINER_FIND(m_WorldHandle->m_Bodies, shared_from_this()));

		m_WorldHandle->m_WorldHandle.DestroyBody(m_BodyHandle);

		m_BodyHandle = nullptr;
		m_WorldHandle = nullptr;
	}

	bool RigidBody2D::IsEnabled() const
	{
		return m_Props.Enabled;
	}

	void RigidBody2D::SetEnabled(bool enabled)
	{
		m_Props.Enabled = enabled;

		if (m_BodyHandle)
			m_BodyHandle->SetEnabled(enabled);
	}

	RigidBody2DType RigidBody2D::GetType() const
	{
		return m_Props.Type;
	}

	void RigidBody2D::SetType(const RigidBody2DType& type)
	{
		m_Props.Type = type;

		if (m_BodyHandle)
			m_BodyHandle->SetType((b2BodyType)((int)type - 1));
	}

	Vector2 RigidBody2D::GetPosition() const
	{
		if (IsDeployed())
		{
			const b2Vec2& pos = m_BodyHandle->GetPosition();
			return { pos.x, pos.y };
		}

		return m_Props.Dynamics.Position;
	}

	void RigidBody2D::SetPosition(const Vector2& position)
	{
		if (IsDeployed())
		{
			m_BodyHandle->SetTransform(
				{ position.x, position.y },
				m_BodyHandle->GetAngle()
			);
			return;
		}

		m_Props.Dynamics.Position = position;
	}

	float RigidBody2D::GetRotation() const
	{
		if (IsDeployed())
			return m_BodyHandle->GetAngle();

		return m_Props.Dynamics.Rotation;
	}

	void RigidBody2D::SetRotation(float rotation)
	{
		if (IsDeployed())
		{
			m_BodyHandle->SetTransform(
				m_BodyHandle->GetPosition(),
				rotation
			);
			return;
		}

		m_Props.Dynamics.Rotation = rotation;
	}

	Vector2 RigidBody2D::GetLinearVelocity() const
	{
		if (IsDeployed())
			return { m_BodyHandle->GetLinearVelocity().x, m_BodyHandle->GetLinearVelocity().y };

		return m_Props.Dynamics.LinearVelocity;
	}

	void RigidBody2D::SetLinearVelocity(const Vector2& velocity)
	{
		if (IsDeployed())
		{
			m_BodyHandle->SetLinearVelocity({ velocity.x, velocity.y });
			return;
		}

		m_Props.Dynamics.LinearVelocity = velocity;
	}

	float RigidBody2D::GetAngularVelocity() const
	{
		if (IsDeployed())
			return m_BodyHandle->GetAngularVelocity();

		return m_Props.Dynamics.AngularVelocity;
	}

	void RigidBody2D::SetAngularVelocity(float velocity)
	{
		if (IsDeployed())
		{
			m_BodyHandle->SetAngularVelocity(velocity);
			return;
		}

		m_Props.Dynamics.AngularVelocity = velocity;
	}

	bool RigidBody2D::IsAwake() const
	{
		if (IsDeployed())
			return m_BodyHandle->IsAwake();

		return m_Props.Dynamics.IsAwake;
	}

	void RigidBody2D::SetAwake(bool isAwake)
	{
		if (IsDeployed())
		{
			m_BodyHandle->SetAwake(isAwake);
			return;
		}

		m_Props.Dynamics.IsAwake = isAwake;
	}

	void RigidBody2D::ApplyLinearImpulse(const Vector2& impulse, const Vector2& point, bool wake)
	{
		m_BodyHandle->ApplyLinearImpulse({ impulse.x, impulse.y }, { point.x, point.y }, wake);
	}

	void RigidBody2D::ApplyLinearImpulseToCenter(const Vector2& impulse, bool wake)
	{
		m_BodyHandle->ApplyLinearImpulseToCenter({ impulse.x, impulse.y }, wake);
	}
}
