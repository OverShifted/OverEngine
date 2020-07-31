#pragma once

#include "PhysicsShape2D.h"
#include "PhysicsMaterial2D.h"

namespace OverEngine
{
	class PhysicsCollider2D
	{
	public:
		PhysicsCollider2D(Ref<PhysicsShape2D> shape = CreateRef<PhysicsShape2D>(), const PhysicsMaterial2D& material = PhysicsMaterial2D(), bool isTrigger = false)
			: m_Shape(shape), m_Material(material), m_IsTrigger(isTrigger)
		{
		}

		Ref<PhysicsShape2D> GetShape() { return m_Shape; }
		const Ref<PhysicsShape2D> GetShape() const { return m_Shape; }

		void SetShape(const Ref<PhysicsShape2D>& shape) { m_Shape = shape; }

		bool IsTrigger() const { return m_IsTrigger; }
		void SetIsTrigger(bool isTrigger) { m_IsTrigger = isTrigger; }

		PhysicsMaterial2D& GetMaterial() { return m_Material; }
		const PhysicsMaterial2D& GetMaterial() const { return m_Material; }

	private:
		Ref<PhysicsShape2D> m_Shape;
		PhysicsMaterial2D m_Material;
		bool m_IsTrigger;
	};
}