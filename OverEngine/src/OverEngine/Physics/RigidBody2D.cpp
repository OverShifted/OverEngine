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
	OE_REFLECT_ENUM_CLASS_BEGIN(RigidBody2DType)
	OE_REFLECT_ENUM_CLASS_VALUE(None, 0)
	OE_REFLECT_ENUM_CLASS_VALUE(Static, 1)
	OE_REFLECT_ENUM_CLASS_VALUE(Kinematic, 2)
	OE_REFLECT_ENUM_CLASS_VALUE(Dynamic, 3)
	OE_REFLECT_ENUM_CLASS_END()

	OE_REFLECT_STRUCT_BEGIN(RigidBody2DProps)
	OE_REFLECT_STRUCT_MEMBER(Type)
	OE_REFLECT_STRUCT_MEMBER(InitialLinearVelocity)
	OE_REFLECT_STRUCT_MEMBER(InitialLinearVelocity)
	OE_REFLECT_STRUCT_MEMBER(LinearDamping)
	OE_REFLECT_STRUCT_MEMBER(AngularDamping)
	OE_REFLECT_STRUCT_MEMBER(AllowSleep)
	OE_REFLECT_STRUCT_MEMBER(IsInitiallyAwake)
	OE_REFLECT_STRUCT_MEMBER(FixedRotation)
	OE_REFLECT_STRUCT_MEMBER(GravityScale)
	OE_REFLECT_STRUCT_MEMBER(Bullet)
	OE_REFLECT_STRUCT_END()

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

		def.position.Set(m_Props.InitialPosition.x, m_Props.InitialPosition.y);
		def.angle = m_Props.InitialRotation;

		def.linearVelocity.Set(m_Props.InitialLinearVelocity.x, m_Props.InitialLinearVelocity.y);
		def.angularVelocity = m_Props.InitialAngularVelocity;

		def.linearDamping = m_Props.LinearDamping;
		def.angularVelocity = m_Props.AngularDamping;

		def.allowSleep = m_Props.AllowSleep;
		def.awake = m_Props.IsInitiallyAwake;
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

	RigidBody2DType RigidBody2D::GetType()
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

		return m_Props.InitialPosition;
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

		m_Props.InitialPosition = position;
	}

	float RigidBody2D::GetRotation() const
	{
		if (IsDeployed())
			return m_BodyHandle->GetAngle();

		return m_Props.InitialRotation;
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

		m_Props.InitialRotation = rotation;
	}

	Vector2 RigidBody2D::GetLinearVelocity() const
	{
		if (IsDeployed())
			return { m_BodyHandle->GetLinearVelocity().x, m_BodyHandle->GetLinearVelocity().y };

		return m_Props.InitialLinearVelocity;
	}

	void RigidBody2D::SetLinearVelocity(const Vector2& velocity)
	{
		if (IsDeployed())
		{
			m_BodyHandle->SetLinearVelocity({ velocity.x, velocity.y });
			return;
		}

		m_Props.InitialLinearVelocity = velocity;
	}

	float RigidBody2D::GetAngularVelocity() const
	{
		if (IsDeployed())
			return m_BodyHandle->GetAngularVelocity();

		return m_Props.InitialAngularVelocity;
	}

	void RigidBody2D::SetAngularVelocity(float velocity)
	{
		if (IsDeployed())
		{
			m_BodyHandle->SetAngularVelocity(velocity);
			return;
		}

		m_Props.InitialAngularVelocity = velocity;
	}

	bool RigidBody2D::IsAwake() const
	{
		if (IsDeployed())
			return m_BodyHandle->IsAwake();

		return m_Props.IsInitiallyAwake;
	}

	void RigidBody2D::SetIsAwake(bool isAwake)
	{
		if (IsDeployed())
		{
			m_BodyHandle->SetAwake(isAwake);
			return;
		}

		m_Props.IsInitiallyAwake = isAwake;
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
