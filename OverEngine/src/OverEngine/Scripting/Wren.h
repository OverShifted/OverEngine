#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Scene/Entity.h"

#include <wrenpp/Wren++.h>

extern "C" {
	#include <wren_value.h>
}

namespace OverEngine
{
	class WrenScriptClass;
	class Wren : public std::enable_shared_from_this<Wren>
	{
	public:
		Wren();
		~Wren();

		Wren(const Wren&) = delete;
		Wren& operator=(const Wren&) = delete;

		inline WrenHandle* GetOnCreateMethod()     const { return m_OnCreateMethod; }
		inline WrenHandle* GetOnDestroyMethod()    const { return m_OnDestroyMethod; }
		inline WrenHandle* GetOnUpdateMethod()     const { return m_OnUpdateMethod; }
		inline WrenHandle* GetOnLateUpdateMethod() const { return m_OnLateUpdateMethod; }

		inline WrenHandle* GetOnCollisionEnterMethod() const { return m_OnCollisionEnterMethod; }
		inline WrenHandle* GetOnCollisionExitMethod()  const { return m_OnCollisionExitMethod; }

		inline ::WrenVM* GetRaw() const { return &*m_VM; }
		inline wrenpp::VM& GetWrenpp() { return m_VM; }

		WrenHandle* CallHandle(const char* signature) const;
		WrenHandle* GetVariable(const char* moduleName, const char* variableName);

		String ToString(WrenHandle* handle) const;
		String ToString(Value value) const;

		template<typename... Args>
		WrenInterpretResult Call(WrenHandle* reciver, WrenHandle* method, Args... args) const
		{
			// Copied from Wrenpp
			constexpr const std::size_t arity = sizeof...(Args);
			wrenEnsureSlots(m_VM, arity + 1u);
			wrenSetSlotHandle(m_VM, 0, reciver);

			wrenpp::detail::passArgumentsToWren(m_VM, std::make_tuple(args...), std::make_index_sequence<arity>{});
			return wrenCall(m_VM, method);
		}

		// TODO: run code on custom module.
		inline wrenpp::Result ExecuteString(const char* string) { return m_VM.executeString(string); }
		inline wrenpp::Result LoadModule(const char* moduleName) { return m_VM.executeModule(moduleName); }
		wrenpp::ModuleContext beginModule(const String& name) { return m_VM.beginModule(name); }

		inline Ref<WrenScriptClass> GetScriptClass(const char* moduleName, const char* className)
		{
			return CreateRef<WrenScriptClass>(shared_from_this(), moduleName, className);
		}

		inline WrenHandle* GetScheduler() { return m_Scheduler; }

		const Vector<String>& GetFields(ObjClass* klass) const;

	private:
		// Implemented in WrenBindings.cpp
		void InitializeBindings();
		
		void AddSuperClassFields(ObjClass* klass) const;

	private:
		wrenpp::VM m_VM;
		mutable UnorderedMap<String, std::pair<bool, Vector<String>>> m_FieldNames;

		WrenHandle* m_Scheduler;

		// Call Handles
		mutable UnorderedMap<String, WrenHandle*> m_CallHandles;

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
		WrenScriptClass(const Ref<Wren>& vm, const char* moduleName, const char* className);
		~WrenScriptClass();

		WrenScriptClass(const WrenScriptClass&) = delete;
		WrenScriptClass& operator=(const WrenScriptClass&) = delete;

		Ref<WrenScriptInstance> Construct(const Entity& entity) const;

	private:
		Ref<Wren> m_VM;
		WrenHandle* m_ClassHandle;

		// TODO: move to Wren
		WrenHandle* m_ConstructorHandle;
	};

	class WrenScriptInstance
	{
	public:
		WrenScriptInstance(const Ref<Wren>& vm, WrenHandle* instanceHandle);
		~WrenScriptInstance();

		WrenScriptInstance(const WrenScriptInstance&) = delete;
		WrenScriptInstance& operator=(const WrenScriptInstance&) = delete;

		WrenInterpretResult OnCreate() const;
		WrenInterpretResult OnDestroy() const;
		WrenInterpretResult OnUpdate(float delta) const;
		WrenInterpretResult OnLateUpdate(float delta) const;

		WrenInterpretResult OnCollisionEnter() const;
		WrenInterpretResult OnCollisionExit() const;

		template<typename... Args>
		WrenInterpretResult Call(const char* sig, Args... args) const
		{
			return Call(m_VM->CallHandle(sig), args...);
		}

		template<typename... Args>
		WrenInterpretResult Call(WrenHandle* method, Args... args) const
		{
			return m_VM->Call(m_InstanceHandle, method, args...);
		}

		// Dump field data to log.
		void Inspect() const;

	private:
		Ref<Wren> m_VM;
		WrenHandle* m_InstanceHandle;
	};
}
