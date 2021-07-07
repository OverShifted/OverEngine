#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Runtime/Reflection/TypeInfo.h"

namespace OverEngine
{
	class Action
	{
	public:
		Action(const SelfContainedPropertySetter& setter, const Variant& firstValue, const Variant& secondValue)
			: m_Setter(setter), m_FirstValue(firstValue), m_SecondValue(secondValue)
		{
		}

		~Action() = default;

		void Perform() { m_Setter(m_SecondValue); }
		void Revert() { m_Setter(m_FirstValue); }

	private:
		SelfContainedPropertySetter m_Setter;
		Variant m_FirstValue;
		Variant m_SecondValue;
	};

	class ActionStack
	{
	public:
		ActionStack() { s_ActiveInstance = this; };

		static ActionStack& GetActiveInstance() { return *s_ActiveInstance; }

		void Reset();
		void Do(Ref<Action> action, bool perform = true);

		Ref<Action> Undo();
		Ref<Action> Redo();

	private:
		static ActionStack* s_ActiveInstance;

		Vector<Ref<Action>> m_UndoStack;
		Vector<Ref<Action>> m_RedoStack;
	};
}
