#pragma once

#include "pcheader.h"
#include "OverEngine/Core/Core.h"

#include "Input.h"

#include "OverEngine/Events/KeyEvent.h"

#define OE_BIND_INPUT_ACTION_CALLBACK_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace OverEngine
{
	class InputTrigger
	{
	public:
		InputTrigger()
			: m_Key(KeyCode::KEY_NONE), m_ActivePress(false), m_ActiveRelease(false), m_ActiveRepeat(false),
				m_PressedThisFrame(false), m_PressedLastFrame(false)
		{
		}

		InputTrigger(KeyCode key, bool activePress = true, bool activeRelease = true, bool activeRepeat = false)
			: m_Key(key), m_ActivePress(activePress), m_ActiveRelease(activeRelease), m_ActiveRepeat(activeRepeat),
				m_PressedThisFrame(false), m_PressedLastFrame(false)
		{
		}

		bool Check();

	private:
		KeyCode m_Key;
		bool m_ActivePress, m_ActiveRelease, m_ActiveRepeat;
		bool m_PressedThisFrame, m_PressedLastFrame;
	};

	class InputBinding
	{
	public:
		enum class BindingMode
		{
			TriggerBinding, AxisBinding, DpadBinding
		};

		InputBinding(InputTrigger trigger)
		{
			m_triggers[0] = trigger;
			m_mode = BindingMode::TriggerBinding;
		}

		InputBinding(InputTrigger triggerMin, InputTrigger triggerMax)
		{
			m_triggers[0] = triggerMin;
			m_triggers[1] = triggerMax;
			m_mode = BindingMode::AxisBinding;
		}

		InputBinding(InputTrigger triggerXMin, InputTrigger triggerXMax, InputTrigger triggerYMin, InputTrigger triggerYMax)
		{
			m_triggers[0] = triggerXMin;
			m_triggers[1] = triggerXMax;
			m_triggers[2] = triggerYMin;
			m_triggers[3] = triggerYMax;
			m_mode = BindingMode::DpadBinding;
		}

		bool Check();

	private:
		BindingMode m_mode;
		InputTrigger m_triggers[4];
	};

	class InputAction
	{
	public:
		struct TriggerInfo
		{
			InputBinding::BindingMode bindingMode;

			float x = 0.0f, y = 0.0f;

			template<typename T>
			T ReadValue()
			{
				if (bindingMode == InputBinding::BindingMode::TriggerBinding)
					return T(true);
				else if (bindingMode == InputBinding::BindingMode::AxisBinding)
					return T(x);
				else
					return T(x, y);
			}
		};

		using InputActionCallBackFn = std::function<void(TriggerInfo&)>;

		InputAction()
		{
		}

		void AddBinding(const Ref<InputBinding>& binding);

		void Check();

		void AddCallBack(InputActionCallBackFn callback);

	private:
		Vector<Ref<InputBinding>> m_Bindings;
		Vector<InputActionCallBackFn> m_CallBacks;
	};

	class InputActionMap
	{
	public:
		void OnUpdate();
		void AddAction(const Ref<InputAction>& action);
	private:
		Vector<Ref<InputAction>> m_Actions;
	};

	class InputSystem
	{
	public:
		InputSystem() = delete;

		static void AddInputActionMap(const Ref<InputActionMap>& inputActionMap);

		static void OnUpdate();
		static void OnKeyPressEvent(KeyPressedEvent& e);
		static void OnKeyReleaseEvent(KeyReleasedEvent& e);

	private:
		static Vector<Ref<InputActionMap>> m_InputActionMaps;
	};
}