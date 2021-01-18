#pragma once

#include "OverEngine/Core/Core.h"

namespace OverEngine
{
	class Collider2D;
	struct Collision2D
	{
		Ref<Collider2D> ColliderA;
		Ref<Collider2D> ColliderB;
	};
}
