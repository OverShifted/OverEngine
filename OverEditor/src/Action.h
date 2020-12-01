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

	struct BoolEditAction : public Action
	{
		using GetterFn = std::function<float()>;
		using SetterFn = std::function<void(const float&)>;

		BoolEditAction() = default;
		BoolEditAction(const GetterFn& getter, const SetterFn& setter)
			: Getter(getter), Setter(setter) {}

		virtual void Perform()
		{
			Setter(!Getter());
		}

		virtual void Revert()
		{
			Perform();
		}

	private:
		GetterFn Getter;
		SetterFn Setter;
	};

	struct IntEditAction : public Action
	{
		using GetterFn = std::function<int()>;
		using SetterFn = std::function<void(const int&)>;

		IntEditAction() = default;
		IntEditAction(const int& delta, const GetterFn& getter, const SetterFn& setter)
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
		int Delta;
		GetterFn Getter;
		SetterFn Setter;
	};

	struct FloatEditAction : public Action
	{
		using GetterFn = std::function<float()>;
		using SetterFn = std::function<void(const float&)>;

		FloatEditAction() = default;
		FloatEditAction(const float& delta, const GetterFn& getter, const SetterFn& setter)
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
		float Delta;
		GetterFn Getter;
		SetterFn Setter;
	};

	struct Vector2EditAction : public Action
	{
		using GetterFn = std::function<Vector2()>;
		using SetterFn = std::function<void(const Vector2&)>;

		Vector2EditAction() = default;
		Vector2EditAction(const Vector2& delta, const GetterFn& getter, const SetterFn& setter)
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
		Vector2 Delta;
		GetterFn Getter;
		SetterFn Setter;
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

	struct Vector4EditAction : public Action
	{
		using GetterFn = std::function<Vector4()>;
		using SetterFn = std::function<void(const Vector4&)>;

		Vector4EditAction() = default;
		Vector4EditAction(const Vector4& delta, const GetterFn& getter, const SetterFn& setter)
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
		Vector4 Delta;
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
