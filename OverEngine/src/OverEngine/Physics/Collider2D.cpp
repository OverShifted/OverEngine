#include "pcheader.h"
#include "Collider2D.h"
#include "RigidBody2D.h"

#include "OverEngine/Scene/TransformComponent.h"

#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

namespace OverEngine
{
	Ref<BoxCollisionShape2D> BoxCollisionShape2D::Create(const Vector2& size, const Vector2& offset, float rotation)
	{
		return CreateRef<BoxCollisionShape2D>(size, offset, rotation);
	}

	BoxCollisionShape2D::BoxCollisionShape2D(const Vector2& size, const Vector2& offset, float rotation)
		: m_Size(size), m_Rotation(rotation)
	{
	}

	void BoxCollisionShape2D::Invalidate(const Mat4x4& transform)
	{
		m_Shape.SetAsBox(
			m_Size.x / 2.0f,
			m_Size.y / 2.0f,
			{ m_Offset.x, m_Offset.y },
			m_Rotation
		);

		std::array<b2Vec2, b2_maxPolygonVertices> transformedVertices;

		for (int i = 0; i < m_Shape.m_count; i++)
		{
			b2Vec2 vertex = m_Shape.m_vertices[i];
			Vector4 transformedVertex = Vector4{vertex.x, vertex.y, 0.0f, 0.0f} * transform;
			transformedVertices[i] = b2Vec2(transformedVertex.x, transformedVertex.y);
		}

		m_Shape.Set(transformedVertices.data(), m_Shape.m_count);
	}

	Ref<CircleCollisionShape2D> CircleCollisionShape2D::Create(float radius)
	{
		return CreateRef<CircleCollisionShape2D>(radius);
	}

	CircleCollisionShape2D::CircleCollisionShape2D(float radius)
		: m_Radius(radius)
	{
	}

	void CircleCollisionShape2D::Invalidate(const Mat4x4& transform)
	{
		// TODO: Change size
		m_Shape.m_p.Set(m_Offset.x, m_Offset.y);
		m_Shape.m_radius = m_Radius;
	}

	Ref<Collider2D> Collider2D::Create(const Collider2DProps& props)
	{
		return CreateRef<Collider2D>(props);
	}

	Collider2D::Collider2D(const Collider2DProps& props)
		: m_Props(props)
	{
	}

	Collider2D::~Collider2D()
	{
		if (m_FixtureHandle && m_BodyHandle && m_BodyHandle->m_BodyHandle)
			UnDeploy();
	}

	void Collider2D::Deploy(RigidBody2D* rigidBody)
	{
		rigidBody->m_Colliders.push_back(shared_from_this());
		m_BodyHandle = rigidBody;
		Invalidate();
	}

	void Collider2D::UnDeploy()
	{
		m_BodyHandle->m_Colliders.erase(STD_CONTAINER_FIND(m_BodyHandle->m_Colliders, shared_from_this()));
		m_BodyHandle->m_BodyHandle->DestroyFixture(m_FixtureHandle);

		m_FixtureHandle = nullptr;
		m_BodyHandle = nullptr;
	}

	void Collider2D::Invalidate()
	{
		OE_CORE_ASSERT(m_BodyHandle && m_BodyHandle->m_BodyHandle, "Cannot (re)build a collider without a body or with an undeployed one.");
		OE_CORE_ASSERT(m_Props.Shape, "Cannot (re)build a collider with empty shape!");

		if (m_FixtureHandle)
			m_BodyHandle->m_BodyHandle->DestroyFixture(m_FixtureHandle);

		b2FixtureDef def;

		auto& bodyTransform = m_BodyHandle->GetProps().AttachedEntity.GetComponent<TransformComponent>();
		Mat4x4 shapeTransform = glm::inverse(
			glm::translate(Mat4x4(1.0f), { bodyTransform.GetPosition().x, bodyTransform.GetPosition().y, 0.0f }) *
			glm::rotate(Mat4x4(1.0f), glm::radians(bodyTransform.GetLocalEulerAngles().z), Vector3(0, 0, 1))
		) * m_Props.AttachedEntity.GetComponent<TransformComponent>().GetLocalToWorld();

		def.shape                = m_Props.Shape->GetBox2DShape(shapeTransform);
		def.friction             = m_Props.Friction;
		def.restitution          = m_Props.Bounciness;
		def.restitutionThreshold = m_Props.BouncinessThreshold;
		def.density              = m_Props.Density;
		def.isSensor             = m_Props.IsTrigger;
		def.userData.pointer     = (uintptr_t)this;

		m_FixtureHandle = m_BodyHandle->m_BodyHandle->CreateFixture(&def);
	}
}
