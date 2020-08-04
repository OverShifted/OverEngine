#pragma once

#include "PhysicsShape2D.h"
#include "PhysicsMaterial2D.h"

namespace OverEngine
{
	class Scene;

	class PhysicsCollider2D
	{
	public:
		PhysicsCollider2D(Ref<PhysicsShape2D> shape = CreateRef<PhysicsShape2D>(), const PhysicsMaterial2D& material = PhysicsMaterial2D(), bool isTrigger = false)
			: m_Shape(shape), m_Material(material), m_IsTrigger(isTrigger)
		{
		}

		const Ref<PhysicsShape2D> GetShape() const { return m_Shape; }
		Ref<PhysicsShape2D> GetShape() { return m_Shape; }
		void SetShape(const Ref<PhysicsShape2D>& shape) { m_Shape = shape; m_Changed = true; }

		bool IsTrigger() const { return m_IsTrigger; }
		void SetIsTrigger(bool isTrigger) { m_IsTrigger = isTrigger; }

		const PhysicsMaterial2D& GetMaterial() const { return m_Material; }
		PhysicsMaterial2D& GetMaterial() { return m_Material; }
		void SetMaterial(const PhysicsMaterial2D& material) { m_Material = material; }

		bool IsEnabled() const { return m_Enabled; }
		void SetEnabled(bool enabled) { m_Enabled = enabled; m_Changed = true; }
	private:
		Ref<PhysicsShape2D> m_Shape;
		PhysicsMaterial2D m_Material;
		bool m_IsTrigger;

		bool m_Enabled;
		bool m_Changed;

		friend class Scene;
	};
}