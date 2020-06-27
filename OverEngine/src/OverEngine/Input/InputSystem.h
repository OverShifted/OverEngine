#pragma once

#include "pcheader.h"
#include "OverEngine/Core/Core.h"

#include "Input.h"

#include "OverEngine/Events/KeyEvent.h"

namespace OverEngine
{
	enum class InputActionMode
	{
		TriggerBinding, AxisBinding, DpadBinding
	};

	enum class InputActionType
	{
		Button, PassThrough
	};

	class InputTrigger
	{
	public:
		InputTrigger()
			: m_Key(KeyCode::None), m_ActivePress(true), m_ActiveRelease(true), m_ActiveRepeat(true),
				m_PressedLastFrame(false)
		{
		}

		InputTrigger(KeyCode key, bool activePress = true, bool activeRelease = true, bool activeRepeat = true)
			: m_Key(key), m_ActivePress(activePress), m_ActiveRelease(activeRelease), m_ActiveRepeat(activeRepeat),
				m_PressedLastFrame(false)
		{
		}

		float Check(bool passThrough);
	private:
		KeyCode m_Key;
		bool m_ActivePress, m_ActiveRelease, m_ActiveRepeat;
		bool m_PressedLastFrame;
	};

	class InputBinding
	{
	public:
		InputBinding() {}

		InputBinding(InputTrigger trigger)
		{
			m_Triggers[0] = trigger;
			m_Mode = InputActionMode::TriggerBinding;
		}

		InputBinding(InputTrigger triggerMin, InputTrigger triggerMax)
		{
			m_Triggers[0] = triggerMin;
			m_Triggers[1] = triggerMax;
			m_Mode = InputActionMode::AxisBinding;
		}

		InputBinding(InputTrigger triggerXMin, InputTrigger triggerXMax, InputTrigger triggerYMin, InputTrigger triggerYMax)
		{
			m_Triggers[0] = triggerXMin;
			m_Triggers[1] = triggerXMax;
			m_Triggers[2] = triggerYMin;
			m_Triggers[3] = triggerYMax;
			m_Mode = InputActionMode::DpadBinding;
		}

		InputBinding(const std::initializer_list<InputTrigger>& triggersInitializerList)
		{
			Vector<InputTrigger> triggers(triggersInitializerList);

			switch (triggers.size())
			{
			case 1:
				m_Triggers[0] = triggers[0];
				m_Mode = InputActionMode::TriggerBinding;
				break;
			case 2:
				m_Triggers[0] = triggers[0];
				m_Triggers[1] = triggers[1];
				m_Mode = InputActionMode::AxisBinding;
				break;
			case 4:
				m_Triggers[0] = triggers[0];
				m_Triggers[1] = triggers[1];
				m_Triggers[2] = triggers[2];
				m_Triggers[3] = triggers[3];
				m_Mode = InputActionMode::DpadBinding;
				break;
			default:
				OE_CORE_ASSERT(false, "{0} is too many for an InputBinding!", triggers.size())
					break;
			}
		}

		std::pair<float, float> Check(bool passThrough);

		inline InputActionMode GetMode() { return m_Mode; }
	private:
		InputActionMode m_Mode;
		InputTrigger m_Triggers[4];
	};

	class InputAction
	{
	public:
		struct TriggerInfo
		{
			InputActionMode bindingMode;

			float x = 0.0f, y = 0.0f;

			template<typename T>
			T ReadValue()
			{
				if (bindingMode == InputActionMode::TriggerBinding)
					return T(x);
				else if (bindingMode == InputActionMode::AxisBinding)
					return T(x);
				else
					return T(x, y);
			}
		};

		using InputActionCallBackFn = std::function<void(TriggerInfo&)>;

		InputAction(InputActionType type, const std::initializer_list<InputBinding>& bindings)
			: m_Bindings(bindings), m_Mode(m_Bindings[0].GetMode()), m_Type(type)
		{
		}

		void AddBinding(const InputBinding& binding);
		void AddCallBack(InputActionCallBackFn callback);

		void Check();
	private:
		Vector<InputBinding> m_Bindings;
		Vector<InputActionCallBackFn> m_CallBacks;

		InputActionMode m_Mode;
		InputActionType m_Type;
		std::pair<float, float> m_LastValue;
	};

	class InputActionMap;

	class InputSystem
	{
	public:
		InputSystem() = delete;

		static void AddInputActionMap(const Ref<InputActionMap>& inputActionMap);

		static void OnUpdate();
	private:
		static Vector<Ref<InputActionMap>> m_InputActionMaps;
	};

	class InputActionMap
	{
	public:
		void OnUpdate();
		void AddAction(const InputAction& action);
	public:
		static Ref<InputActionMap> Create()
		{
			auto actionMap = CreateRef<InputActionMap>();
			InputSystem::AddInputActionMap(actionMap);
			return actionMap;
		}
	private:
		Vector<InputAction> m_Actions;
	};
}