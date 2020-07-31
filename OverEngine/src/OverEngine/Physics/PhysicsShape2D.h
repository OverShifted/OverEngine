#pragma once

#include <box2d/b2_shape.h>

namespace OverEngine
{
	class PhysicsBody2D;

	enum class PhysicsShape2DType
	{
		Circle, Polygon
	};

	class PhysicsShape2D
	{
	public:
		PhysicsShape2D() = default;

		void SetAsCircle(const Vector2& position, float radius);
		void SetAsBox(const Vector2& size, float rotation = 0.0f, const Vector2& center = { 0.0f, 0.0f });
	private:
		b2Shape* m_ShapeHandle;
		void* m_ShapeBasedHandle;

		PhysicsShape2DType m_Type;

		friend class PhysicsBody2D;
	};
}