#include "pcheader.h"

#include "InputSystem.h"

namespace OverEngine
{
	//// InputTrigger /////////////////////////////

	bool InputTrigger::Check()
	{
		m_PressedThisFrame = Input::IsKeyPressed(m_Key);

		if (m_ActivePress)
		{
			if (!m_PressedLastFrame && m_PressedThisFrame)
			{
				m_PressedLastFrame = m_PressedThisFrame;
				return true;
			}
		}

		if (m_ActiveRelease)
		{
			if (!m_PressedThisFrame && m_PressedLastFrame)
			{
				m_PressedLastFrame = m_PressedThisFrame;
				return true;
			}
		}

		m_PressedLastFrame = m_PressedThisFrame;
		return false;
	}

	//// InputBinding /////////////////////////////

	bool InputBinding::Check()
	{
		if (m_mode == BindingMode::TriggerBinding)
			return m_triggers[0].Check();

		return false;
	}

	//// InputAction //////////////////////////////

	void InputAction::Check()
	{
		bool _triggered = false;
		for (const auto& binding : m_Bindings)
		{
			_triggered = binding->Check();
			if (_triggered)
				break;
		}

		if (!_triggered)
			return;

		TriggerInfo info = TriggerInfo();

		for (InputActionCallBackFn callback : m_CallBacks)
		{
			callback(info);
		}
	}

	void InputAction::AddBinding(const Ref<InputBinding>& binding)
	{
		m_Bindings.push_back(binding);
	}

	void InputAction::AddCallBack(InputActionCallBackFn callback)
	{
		m_CallBacks.push_back(callback);
	}

	//// InputActionMap ///////////////////////////

	void InputActionMap::OnUpdate()
	{
		for (const auto& action : m_Actions)
		{
			action->Check();
		}
	}

	void InputActionMap::AddAction(const Ref<InputAction>& action)
	{
		m_Actions.push_back(action);
	}

	//// InputSystem //////////////////////////////

	void InputSystem::OnUpdate()
	{
		for (const auto& map : m_InputActionMaps)
		{
			map->OnUpdate();
		}
	}

	void InputSystem::AddInputActionMap(const std::shared_ptr<InputActionMap>& inputActionMap)
	{
		m_InputActionMaps.push_back(inputActionMap);
	}

	std::vector<std::shared_ptr<OverEngine::InputActionMap>> InputSystem::m_InputActionMaps;

}