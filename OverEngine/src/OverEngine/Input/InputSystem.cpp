#include "pcheader.h"

#include "InputSystem.h"

namespace OverEngine
{
	///////////////////////////////////////////////
	//// InputTrigger /////////////////////////////
	///////////////////////////////////////////////

	float InputTrigger::Check(bool passThrough)
	{
		bool pressedThisFrame = Input::IsKeyPressed(m_Key);

		if (passThrough)
		{
			m_PressedLastFrame = pressedThisFrame;
			return pressedThisFrame;
		}

		if (m_ActivePress)
		{
			if (!m_PressedLastFrame && pressedThisFrame)
			{
				m_PressedLastFrame = pressedThisFrame;
				return true;
			}
		}

		if (m_ActiveRelease)
		{
			if (!pressedThisFrame && m_PressedLastFrame)
			{
				m_PressedLastFrame = pressedThisFrame;
				return true;
			}
		}

		m_PressedLastFrame = pressedThisFrame;
		return false;
	}

	///////////////////////////////////////////////
	//// InputBinding /////////////////////////////
	///////////////////////////////////////////////

	std::pair<float, float> InputBinding::Check(bool passThrough)
	{
		if (m_Mode == InputActionMode::TriggerBinding)
			return { (float)(m_Triggers[0].Check(passThrough)), 0.0f };

		if (m_Mode == InputActionMode::AxisBinding)
			return { (float)(m_Triggers[1].Check(passThrough) - m_Triggers[0].Check(passThrough)), 0.0f };

		if (m_Mode == InputActionMode::DpadBinding)
			return {
				(float)(m_Triggers[1].Check(passThrough) - m_Triggers[0].Check(passThrough)),
				(float)(m_Triggers[3].Check(passThrough) - m_Triggers[2].Check(passThrough))
			};

		return { 0.0f, 0.0f };
	}

	///////////////////////////////////////////////
	//// InputAction //////////////////////////////
	///////////////////////////////////////////////

	void InputAction::Check()
	{
		if (m_Type == InputActionType::PassThrough)
		{
			std::pair<float, float> value = { 0.0f, 0.0f };
			for (auto& binding : m_Bindings)
			{
				value.first  += binding.Check(true).first;
				value.second += binding.Check(true).second;
			}
			value.first = Math::Clamp(value.first, -1.0f, 1.0f);
			value.second = Math::Clamp(value.second, -1.0f, 1.0f);

			TriggerInfo info = TriggerInfo();
			info.bindingMode = m_Mode;
			info.x = value.first;
			info.y = value.second;

			for (InputActionCallBackFn callback : m_CallBacks)
				callback(info);
		}
		else
		{
			if (m_Mode != InputActionMode::TriggerBinding)
			{
				std::pair<float, float> value = { 0.0f, 0.0f };
				for (auto& binding : m_Bindings)
				{
					value.first += binding.Check(true).first;
					value.second += binding.Check(true).second;
				}
				value.first = Math::Clamp(value.first, -1.0f, 1.0f);
				value.second = Math::Clamp(value.second, -1.0f, 1.0f);

				if (m_LastValue.first == value.first && m_LastValue.second == value.second)
				{
					return;
					m_LastValue = value;
				}
				m_LastValue = value;

				TriggerInfo info = TriggerInfo();
				info.bindingMode = m_Mode;
				info.x = value.first;
				info.y = value.second;

				for (InputActionCallBackFn callback : m_CallBacks)
					callback(info);
			}
			else
			{
				std::pair<float, float> value = { 0.0f, 0.0f };
				for (auto& binding : m_Bindings)
				{
					value.first += binding.Check(false).first;
				}
				value.first = Math::Clamp(value.first, 0.0f, 1.0f);
				m_LastValue = value;

				if (!value.first)
				{
					return;
					m_LastValue = value;
				}

				TriggerInfo info = TriggerInfo();
				info.bindingMode = m_Mode;
				info.x = value.first;

				for (InputActionCallBackFn callback : m_CallBacks)
					callback(info);
				
			}
		}
	}

	void InputAction::AddBinding(const InputBinding& binding)
	{
		m_Bindings.push_back(binding);
	}

	void InputAction::AddCallBack(InputActionCallBackFn callback)
	{
		m_CallBacks.push_back(callback);
	}

	///////////////////////////////////////////////
	//// InputActionMap ///////////////////////////
	///////////////////////////////////////////////

	void InputActionMap::OnUpdate()
	{
		for (auto& action : m_Actions)
		{
			action.Check();
		}
	}

	void InputActionMap::AddAction(const InputAction& action)
	{
		m_Actions.push_back(action);
	}

	///////////////////////////////////////////////
	//// InputSystem //////////////////////////////
	///////////////////////////////////////////////

	void InputSystem::OnUpdate()
	{
		for (const auto& map : m_InputActionMaps)
		{
			map->OnUpdate();
		}
	}

	void InputSystem::AddInputActionMap(const Ref<InputActionMap>& inputActionMap)
	{
		m_InputActionMaps.push_back(inputActionMap);
	}

	Vector<Ref<OverEngine::InputActionMap>> InputSystem::m_InputActionMaps;

}