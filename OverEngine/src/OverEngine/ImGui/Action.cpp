#include "pcheader.h"
#include "Action.h"

namespace OverEngine
{
	ActionStack* ActionStack::s_ActiveInstance = nullptr;

	void ActionStack::Reset()
	{
		m_UndoStack.clear();
		m_RedoStack.clear();
	}

	void ActionStack::Do(Ref<Action> action, bool perform)
	{
		if (perform)
			action->Perform();

		m_RedoStack.clear();
		m_UndoStack.push_back(action);
	}

	Ref<Action> ActionStack::Undo()
	{
		if (m_UndoStack.empty())
			return nullptr;

		auto action = m_UndoStack[m_UndoStack.size() - 1];
		action->Revert();

		m_UndoStack.pop_back();
		m_RedoStack.push_back(action);

		return action;
	}

	Ref<Action> ActionStack::Redo()
	{
		if (m_RedoStack.empty())
			return nullptr;

		auto action = m_RedoStack[m_RedoStack.size() - 1];
		action->Perform();

		m_RedoStack.pop_back();
		m_UndoStack.push_back(action);

		return action;
	}
}
