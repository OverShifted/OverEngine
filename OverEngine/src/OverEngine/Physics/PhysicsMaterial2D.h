#pragma once

namespace OverEngine
{
	struct PhysicsMaterial2D
	{
		PhysicsMaterial2D()
		{
			Friction = 0.1f;
			Bounciness = 0.0f;
			Density = 1.0f;
		}

		PhysicsMaterial2D(float friction, float bounciness, float density)
			: Friction(friction), Bounciness(bounciness), Density(density)
		{
		}

		float Friction;
		float Bounciness;
		float Density;
	};
}