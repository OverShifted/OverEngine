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

	struct Vector3EditAction : public Action
	{
		using GetterFn = std::function<Vector3()>;
		using SetterFn = std::function<void(const Vector3&)>;

		Vector3EditAction() = default;
		Vector3EditAction(const Vector3& delta, const GetterFn& getter, const SetterFn& setter)
			: Delta(delta), Getter(getter), Setter(setter) {}

		virtual void Perform()
		{
			Setter(Getter() + Delta);
		}

		virtual void Revert()
		{
			Setter(Getter() - Delta);
		}

	private:
		Vector3 Delta;
		GetterFn Getter;
		SetterFn Setter;
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
