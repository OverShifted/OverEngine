#pragma once

#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/Time/TimeStep.h"

#include <box2d/b2_world.h>

namespace OverEngine
{
	class PhysicsBody2D;

	class PhysicsWorld2D
	{
	public:
		PhysicsWorld2D(Vector2 gravity);

		void OnUpdate(TimeStep ts, uint32_t velocityIterations, uint32_t positionIterations);

		Vector2 GetGravity() const;
		void SetGravity(const Vector2& gravity);
	private:
		b2World m_WorldHandle;

		friend class PhysicsBody2D;
	};
}
