#pragma once

#include <OverEngine.h>

namespace OverEditor
{
	using namespace OverEngine;

	struct Action
	{
	public:
		Action() = default;
		virtual ~Action() = default;

		virtual void Perform() {}
		virtual void Revert() {}
	};

	struct EntityTranslationAction : public Action
	{
		EntityTranslationAction() = default;
		EntityTranslationAction(const OverEngine::Entity& entity, const Vector3& delta)
			: Entity(entity), Delta(delta) {}

		virtual void Perform()
		{
			auto& tc = Entity.GetComponent<TransformComponent>();
			tc.SetLocalPosition(tc.GetLocalPosition() + Delta);
		}

		virtual void Revert()
		{
			auto& tc = Entity.GetComponent<TransformComponent>();
			tc.SetLocalPosition(tc.GetLocalPosition() - Delta);
		}

	private:
		OverEngine::Entity Entity;
		Vector3 Delta;
	};

	struct EntityRotationAction : public Action
	{
		EntityRotationAction() = default;
		EntityRotationAction(const OverEngine::Entity& entity, const Vector3& delta)
			: Entity(entity), Delta(delta) {}

		virtual void Perform()
		{
			auto& tc = Entity.GetComponent<TransformComponent>();
			tc.SetLocalEulerAngles(tc.GetLocalEulerAngles() + Delta);
		}

		virtual void Revert()
		{
			auto& tc = Entity.GetComponent<TransformComponent>();
			tc.SetLocalEulerAngles(tc.GetLocalEulerAngles() - Delta);
		}

	private:
		OverEngine::Entity Entity;
		Vector3 Delta;
	};

	struct EntityScaleAction : public Action
	{
		EntityScaleAction() = default;
		EntityScaleAction(const OverEngine::Entity& entity, const Vector3& delta)
			: Entity(entity), Delta(delta) {}

		virtual void Perform()
		{
			auto& tc = Entity.GetComponent<TransformComponent>();
			tc.SetLocalScale(tc.GetLocalScale() + Delta);
		}

		virtual void Revert()
		{
			auto& tc = Entity.GetComponent<TransformComponent>();
			tc.SetLocalScale(tc.GetLocalScale() - Delta);
		}

	private:
		OverEngine::Entity Entity;
		Vector3 Delta;
	};

	class ActionStack
	{
	public:
		ActionStack() = default;

		void Reset();
		void Do(Ref<Action> action, bool perform = true);

		Ref<Action> Undo();
		Ref<Action> Redo();
	private:
		Vector<Ref<Action>> m_UndoStack;
		Vector<Ref<Action>> m_RedoStack;
	};
}
