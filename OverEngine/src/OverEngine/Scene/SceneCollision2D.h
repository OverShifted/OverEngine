#pragma once

#include "OverEngine/Core/Core.h"
#include "Entity.h"
#include "OverEngine/Physics/RigidBody2D.h"
#include "OverEngine/Physics/Collider2D.h"

namespace OverEngine
{
	struct SceneCollision2D
	{
		struct
		{
			OverEngine::Entity Entity;
			Ref<RigidBody2D> RigidBody;
			Ref<Collider2D> Collider;
		} This, Other;
	};
}
