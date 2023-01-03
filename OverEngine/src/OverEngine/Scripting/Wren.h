#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Scene/Entity.h"

#include <wrenpp/Wren++.h>

namespace OverEngine
{
	class WrenScriptClass;
	class WrenVM : public std::enable_shared_from_this<WrenVM>
	{
	public:
		WrenVM();
		~WrenVM();

		WrenVM(const WrenVM&) = delete;
		WrenVM& operator=(const WrenVM&) = delete;

		inline WrenHandle* GetOnCreateMethod()     const { return m_OnCreateMethod; }
		inline WrenHandle* GetOnDestroyMethod()    const { return m_OnDestroyMethod; }
		inline WrenHandle* GetOnUpdateMethod()     const { return m_OnUpdateMethod; }
		inline WrenHandle* GetOnLateUpdateMethod() const { return m_OnLateUpdateMethod; }

		inline WrenHandle* GetOnCollisionEnterMethod() const { return m_OnCollisionEnterMethod; }
		inline WrenHandle* GetOnCollisionExitMethod()  const { return m_OnCollisionExitMethod; }

		inline ::WrenVM* GetRaw() const { return &*m_VM; }
		inline wrenpp::VM& GetWrenpp() { return m_VM; }

		template<typename... Args>
		WrenInterpretResult CallMethod(WrenHandle* reciver, WrenHandle* method, Args... args) const
		{
			::WrenVM *vm = GetRaw();

			// Copied from Wrenpp
			constexpr const std::size_t Arity = sizeof...(Args);
			wrenEnsureSlots(vm, Arity + 1u);
			wrenSetSlotHandle(vm, 0, reciver);

			std::tuple<Args...> tuple = std::make_tuple(args...);
			wrenpp::detail::passArgumentsToWren(vm, tuple, std::make_index_sequence<Arity>{});

			return wrenCall(vm, method);
		}

		inline wrenpp::Result LoadModule(const char* moduleName)
		{
			return m_VM.executeModule(moduleName);
		}

		inline Ref<WrenScriptClass> GetScriptClass(const char* moduleName, const char* className)
		{
			return CreateRef<WrenScriptClass>(shared_from_this(), moduleName, className);
		}

		wrenpp::ModuleContext beginModule(const String& name) { return m_VM.beginModule(name); }

	private:
		// Implemented in WrenBindings.cpp
		void InitializeBindings();

	private:
		wrenpp::VM m_VM;

		// Call Handles
		WrenHandle* m_OnCreateMethod;
		WrenHandle* m_OnDestroyMethod;
		WrenHandle* m_OnUpdateMethod;
		WrenHandle* m_OnLateUpdateMethod;

		WrenHandle* m_OnCollisionEnterMethod;
		WrenHandle* m_OnCollisionExitMethod;
	};

	class WrenScriptInstance;
	class WrenScriptClass
	{
	public:
		WrenScriptClass(const Ref<WrenVM>& vm, const char* moduleName, const char* className);
		~WrenScriptClass();

		WrenScriptClass(const WrenScriptClass&) = delete;
		WrenScriptClass& operator=(const WrenScriptClass&) = delete;

		Ref<WrenScriptInstance> Construct(const Entity& entity) const;

	private:
		Ref<WrenVM> m_VM;
		WrenHandle* m_ClassHandle;
		WrenHandle* m_ConstructorHandle;
	};

	class WrenScriptInstance {
	public:
		WrenScriptInstance(const Ref<WrenVM>& vm, WrenHandle* instanceHandle);
		~WrenScriptInstance();

		WrenScriptInstance(const WrenScriptInstance&) = delete;
		WrenScriptInstance& operator=(const WrenScriptInstance&) = delete;

		void OnCreate() const;
		void OnDestroy() const;
		void OnUpdate(float delta) const;
		void OnLateUpdate(float delta) const;

		void OnCollisionEnter() const;
		void OnCollisionExit() const;

		template<typename... Args>
		WrenInterpretResult CallMethod(const String& sig, Args... args)
		{
			WrenHandle* methodSig = wrenMakeCallHandle(m_VM->GetRaw(), sig.c_str());
			WrenInterpretResult result = m_VM->CallMethod(m_InstanceHandle, methodSig, args...);
			wrenReleaseHandle(m_VM->GetRaw(), methodSig);

			return result;
		}

	private:
		Ref<WrenVM> m_VM;
		WrenHandle* m_InstanceHandle;
	};
}
