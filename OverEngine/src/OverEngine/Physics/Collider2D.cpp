#include "pcheader.h"
#include "Collider2D.h"
#include "RigidBody2D.h"

#include "OverEngine/Core/Runtime/Reflection/Reflection.h"

#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

namespace OverEngine
{
	OE_REFLECT_ENUM_CLASS_BEGIN(Collider2DType)
	OE_REFLECT_ENUM_CLASS_VALUE(Box, 0)
	OE_REFLECT_ENUM_CLASS_VALUE(Circle, 1)
	OE_REFLECT_ENUM_CLASS_END()

	Collider2D::Collider2D(RigidBody2D* body, b2Fixture* fixture, Collider2DType type, Vector2 offset, float rotation, Vector2 sizeHint)
		: m_Type(type), m_Body(body), m_FixtureHandle(fixture), m_Offset(offset), m_Rotation(rotation), m_SizeHint(sizeHint)
	{
		m_FixtureHandle->GetUserData().pointer = (uintptr_t)this;
	}

	void Collider2D::ReShape(float radius)
	{
		OE_CORE_ASSERT(m_Type == Collider2DType::Circle, "Collider2D::ReShape(float radius) is only possible on CircleColliders");

		m_SizeHint.x = radius;

		b2FixtureDef def;

		b2CircleShape shape;
		shape.m_radius = radius;
		shape.m_p.Set(m_Offset.x, m_Offset.y);

		def.shape                = reinterpret_cast<b2Shape*>(&shape);
		def.friction             = m_FixtureHandle->GetFriction();
		def.restitution          = m_FixtureHandle->GetRestitution();
		def.restitutionThreshold = m_FixtureHandle->GetRestitutionThreshold();
		def.density              = m_FixtureHandle->GetDensity();
		def.isSensor             = m_FixtureHandle->IsSensor();
		def.filter               = m_FixtureHandle->GetFilterData();

		m_Body->m_BodyHandle->DestroyFixture(m_FixtureHandle);
		m_FixtureHandle = m_Body->m_BodyHandle->CreateFixture(&def);
	}

	void Collider2D::ReShape(Vector2 size)
	{
		OE_CORE_ASSERT(m_Type == Collider2DType::Box, "Collider2D::ReShape(Vector2 size) is only possible on BoxColliders");

		m_SizeHint = size;

		b2FixtureDef def;

		b2PolygonShape shape;
		shape.SetAsBox(size.x / 2.0f, size.y / 2.0f, { m_Offset.x, m_Offset.y }, m_Rotation);

		def.shape                = (b2Shape*)&shape;
		def.friction             = m_FixtureHandle->GetFriction();
		def.restitution          = m_FixtureHandle->GetRestitution();
		def.restitutionThreshold = m_FixtureHandle->GetRestitutionThreshold();
		def.density              = m_FixtureHandle->GetDensity();
		def.isSensor             = m_FixtureHandle->IsSensor();
		def.filter               = m_FixtureHandle->GetFilterData();

		m_Body->m_BodyHandle->DestroyFixture(m_FixtureHandle);
		m_FixtureHandle = m_Body->m_BodyHandle->CreateFixture(&def);
	}

	void Collider2D::SetOffset(const Vector2& offset)
	{
		m_Offset = offset;
		
		if (m_Type == Collider2DType::Box)
			ReShape(m_SizeHint);
		else if (m_Type == Collider2DType::Circle)
			ReShape(m_SizeHint.x);
	}

	void Collider2D::SetRotation(float rotation)
	{
		m_Rotation = rotation;

		if (m_Type == Collider2DType::Box)
			ReShape(m_SizeHint);
		else if (m_Type == Collider2DType::Circle)
			ReShape(m_SizeHint.x);
	}
}
