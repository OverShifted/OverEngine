#include "pcheader.h"
#include "PhysicsShape2D.h"

#include <box2d/b2_circle_shape.h>
#include <box2d/b2_polygon_shape.h>

namespace OverEngine
{

	void PhysicsShape2D::SetAsCircle(const Vector2& position, float radius)
	{
		m_Type = PhysicsShape2DType::Circle;
		m_ShapeBasedHandle = new b2CircleShape();
		b2CircleShape* circleShape = static_cast<b2CircleShape*>(m_ShapeBasedHandle);

		m_ShapeHandle = static_cast<b2Shape*>(m_ShapeBasedHandle);

		circleShape->m_p.Set(position.x, position.y);
		circleShape->m_radius = radius;
	}

	void PhysicsShape2D::SetAsBox(const Vector2& size, float rotation /*= 0.0f*/, const Vector2& center /*= { 0.0f, 0.0f }*/)
	{
		m_Type = PhysicsShape2DType::Polygon;
		m_ShapeBasedHandle = new b2PolygonShape();
		b2PolygonShape* polygonShape = static_cast<b2PolygonShape*>(m_ShapeBasedHandle);

		m_ShapeHandle = static_cast<b2Shape*>(m_ShapeBasedHandle);

		polygonShape->SetAsBox(size.x / 2, size.y / 2, { center.x, center.y }, glm::radians(rotation));
	}

}