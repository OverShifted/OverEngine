#pragma once

#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/Time/TimeStep.h"

#include "RigidBody2D.h"

#include <box2d/b2_world.h>

namespace OverEngine
{
	class PhysicWorld2D
	{
	public:
		PhysicWorld2D(Vector2 gravity);

		Ref<RigidBody2D> CreateRigidBody(const RigidBody2DProps& props = RigidBody2DProps());
		void DestroyRigidBody(const Ref<RigidBody2D>& rigidBody);

		Vector2 GetGravity() const;
		void SetGravity(const Vector2& gravity);

		void OnUpdate(TimeStep ts, uint32_t velocityIterations, uint32_t positionIterations);
	private:
		b2World m_WorldHandle;

		// Where all _Ref<RigidBody2D>_s are pointing to
		// When a body is destroyed, b2Body* in the
		// RigidBody2D is nullified and reference is removed from this Vector
		Vector<Ref<RigidBody2D>> m_Bodies;
	};
}
