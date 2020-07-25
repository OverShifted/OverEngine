#pragma once

#include "OverEngine/Core/Time/TimeStep.h"

#include <entt.hpp>

namespace OverEngine
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const String& name = String());

		void OnUpdate(TimeStep ts);
	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}