#include "pcheader.h"
#include "Wren.h"

#include <imgui.h>

extern "C" {
	#include <wren_vm.h>
}

namespace WrenSources
{
	#include "Wren/components.wren.inc"
	#include "Wren/entity.wren.inc"
	#include "Wren/imgui.wren.inc"
	#include "Wren/input.wren.inc"
	#include "Wren/keycodes.wren.inc"
	#include "Wren/lib.wren.inc"
	#include "Wren/math.wren.inc"
	#include "Wren/scheduler.wren.inc"
	#include "Wren/script.wren.inc"
	#include "Wren/time.wren.inc"
}

namespace OverEngine
{
	inline const char* ErrorTypeToString(WrenErrorType type)
	{
		switch (type)
		{
		case WREN_ERROR_COMPILE: return "WREN_ERROR_COMPILE";
		case WREN_ERROR_RUNTIME: return "WREN_ERROR_RUNTIME";
		case WREN_ERROR_STACK_TRACE: return "WREN_ERROR_STACK_TRACE";
		default: assert(false); return "";
		}
	}

	Wren::Wren()
		: m_VM()
	{
		// FIXME: Only one VM instance can exist, because there callbacks are static variables.
		wrenpp::VM::writeFn = [](const char* message) { if (strcmp(message, "\n")) OE_INFO("[wren] {}", message); };
		wrenpp::VM::errorFn = [](WrenErrorType type, const char* moduleName, int line, const char* message) -> void {
			const char* typeStr = ErrorTypeToString(type);
			if (moduleName)
				OE_ERROR("{} in {}:{}> {}", typeStr, moduleName, line, message);
			else
				OE_ERROR("{}> {}", typeStr, message);
		};

		// This wont register superclasses fields.
		wrenpp::VM::reportClassFn = [this](ClassInfo* classInfo) {
			this->m_FieldNames[classInfo->name->value] = std::make_pair(false, Vector<String>());
			auto& fieldNames = this->m_FieldNames[classInfo->name->value].second;

			SymbolTable* syms = &classInfo->fields;
			for (ObjString** current = syms->data; current < syms->data + syms->count; current++)
				fieldNames.push_back((*current)->value);
		};

		m_FieldNames["Object"] = std::make_pair(true, Vector<String>());

		wrenpp::VM::loadModuleFn = [](const char* mod) -> char* {
			#define WREN_MOD(m) if (strcmp(mod, #m) == 0) return const_cast<char*>(WrenSources::m##ModuleSource);

			WREN_MOD(components)
			WREN_MOD(entity)
			WREN_MOD(imgui)
			WREN_MOD(input)
			WREN_MOD(keycodes)
			WREN_MOD(lib)
			WREN_MOD(math)
			WREN_MOD(script)
			WREN_MOD(scheduler)
			WREN_MOD(time)
			
			std::string path(mod);
			path += ".wren";
			std::string source;
			try
			{
				source = wrenpp::detail::fileToString(path);
			}
			catch (const std::exception&)
			{
				return NULL;
			}
			char* buffer = (char*)malloc(source.size());
			assert(buffer != nullptr);
			memcpy(buffer, source.c_str(), source.size());
			return buffer;
		};

		InitializeBindings();

		m_Scheduler = GetVariable("scheduler", "Scheduler");
		
		m_OnCreateMethod     = wrenMakeCallHandle(m_VM, "onCreate()");
		m_OnDestroyMethod    = wrenMakeCallHandle(m_VM, "onDestroy()");
		m_OnUpdateMethod     = wrenMakeCallHandle(m_VM, "onUpdate(_)");
		m_OnLateUpdateMethod = wrenMakeCallHandle(m_VM, "onLateUpdate(_)");

		m_OnCollisionEnterMethod = wrenMakeCallHandle(m_VM, "onCollisionEnter()");
		m_OnCollisionExitMethod  = wrenMakeCallHandle(m_VM, "onCollisionExit()");
	}

	Wren::~Wren()
	{
		wrenReleaseHandle(m_VM, m_OnCreateMethod);
		wrenReleaseHandle(m_VM, m_OnDestroyMethod);
		wrenReleaseHandle(m_VM, m_OnUpdateMethod);
		wrenReleaseHandle(m_VM, m_OnLateUpdateMethod);

		wrenReleaseHandle(m_VM, m_OnCollisionEnterMethod);
		wrenReleaseHandle(m_VM, m_OnCollisionExitMethod);

		for (auto& [_, handle] : m_CallHandles)
			wrenReleaseHandle(m_VM, handle);
	}

	WrenHandle* Wren::CallHandle(const char* signature) const
	{
		if (auto handle = m_CallHandles.find(signature); handle != m_CallHandles.end())
			return handle->second;

		WrenHandle* callHandle = wrenMakeCallHandle(m_VM, signature);
		m_CallHandles[signature] = callHandle;
		return callHandle;
	}

	WrenHandle* Wren::GetVariable(const char* moduleName, const char* variableName)
	{
		wrenEnsureSlots(m_VM, 1);
		wrenGetVariable(m_VM, moduleName, variableName, 0);
		return wrenGetSlotType(m_VM, 0) == WREN_TYPE_NULL ? nullptr : wrenGetSlotHandle(m_VM, 0);
	}

	String Wren::ToString(WrenHandle* handle) const
	{
		Call(handle, CallHandle("toString"));
		return String(wrenGetSlotString(m_VM, 0));
	}

	String Wren::ToString(Value value) const
	{
		WrenHandle handle { value, nullptr, nullptr };
		return ToString(&handle);
	}

	const Vector<String>& Wren::GetFields(ObjClass* klass) const
	{
		auto& [hasSuperClassFields, fields] = m_FieldNames.at(klass->name->value);

		if (!hasSuperClassFields)
			AddSuperClassFields(klass);

		return fields;
	}

	void Wren::AddSuperClassFields(ObjClass* klass) const
	{
		auto& [hasSuperClassFields, fields] = m_FieldNames[klass->name->value];

		auto& superClassFields = GetFields(klass->superclass);
		fields.insert(fields.begin(), superClassFields.begin(), superClassFields.end());

		hasSuperClassFields = true;
	}

	WrenScriptClass::WrenScriptClass(const Ref<Wren>& vm, const char* moduleName, const char* className)
		: m_VM(vm)
	{
		m_ClassHandle = m_VM->GetVariable(moduleName, className);
		OE_CORE_ASSERT(m_ClassHandle, "Wren class is null! Maybe it failed to compile.");
		m_ConstructorHandle = wrenMakeCallHandle(m_VM->GetRaw(), "new(_)");
	}

	WrenScriptClass::~WrenScriptClass()
	{
		wrenReleaseHandle(m_VM->GetRaw(), m_ClassHandle);
		wrenReleaseHandle(m_VM->GetRaw(), m_ConstructorHandle);
	}

	Ref<WrenScriptInstance> WrenScriptClass::Construct(const Entity& entity) const
	{
		if (m_VM->Call(m_ClassHandle, m_ConstructorHandle, entity) != WREN_RESULT_SUCCESS)
		{
			OE_CORE_ASSERT(false, "Script instantiation failure!");
			return nullptr;
		}

		WrenHandle* instance = wrenGetSlotHandle(m_VM->GetRaw(), 0);
		return CreateRef<WrenScriptInstance>(m_VM, instance);
	}

	WrenScriptInstance::WrenScriptInstance(const Ref<Wren>& vm, WrenHandle* instanceHandle)
		: m_VM(vm), m_InstanceHandle(instanceHandle)
	{
	}

	WrenScriptInstance::~WrenScriptInstance()
	{
		wrenReleaseHandle(m_VM->GetRaw(), m_InstanceHandle);
	}

	#define IMPL_METHOD_CALL_NO_PARAM(method)                  \
		WrenInterpretResult WrenScriptInstance::method() const \
		{                                                      \
			return Call(m_VM->Get##method##Method());          \
		}

	#define IMPL_METHOD_CALL_WITH_PARAM(method, params, wrenParams)  \
		WrenInterpretResult WrenScriptInstance::method(params) const \
		{                                                            \
			return Call(m_VM->Get##method##Method(), wrenParams);    \
		}

	IMPL_METHOD_CALL_NO_PARAM(OnCreate)
	IMPL_METHOD_CALL_NO_PARAM(OnDestroy)
	IMPL_METHOD_CALL_WITH_PARAM(OnUpdate, float delta, delta)
	IMPL_METHOD_CALL_WITH_PARAM(OnLateUpdate, float delta, delta)
	IMPL_METHOD_CALL_NO_PARAM(OnCollisionEnter)
	IMPL_METHOD_CALL_NO_PARAM(OnCollisionExit)

	void WrenScriptInstance::Inspect() const
	{
		ObjClass* klass = wrenGetClass(m_VM->GetRaw(), m_InstanceHandle->value);
		ObjInstance* instance = AS_INSTANCE(m_InstanceHandle->value);

		auto& fields = m_VM->GetFields(klass);

		for (int i = 0; i < klass->numFields; i++)
		{
			ImGui::TextUnformatted(fields[i].c_str());
			ImGui::SameLine();
			ImGui::TextUnformatted(m_VM->ToString(instance->fields[i]).c_str());
		}
	}
}
