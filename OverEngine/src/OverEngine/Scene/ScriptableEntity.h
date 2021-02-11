#pragma once

#include "Entity.h"
#include "SceneCollision2D.h"

#include "OverEngine/Core/Time/TimeStep.h"

namespace OverEngine
{
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return AttachedEntity.AddComponent<T>();
		}

		template<typename T>
		T& GetComponent()
		{
			return AttachedEntity.GetComponent<T>();
		}

		template<typename T>
		bool HasComponent()
		{
			return AttachedEntity.HasComponent<T>();
		}

		template<typename T>
		void RemoveComponent()
		{
			AttachedEntity.RemoveComponent<T>();
		}

		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(TimeStep ts) {}

		virtual void OnCollisionEnter(const SceneCollision2D& collision) {}
		virtual void OnCollisionExit(const SceneCollision2D& collision) {}
	protected:
		Entity AttachedEntity;
		friend class Scene;
	};
}
