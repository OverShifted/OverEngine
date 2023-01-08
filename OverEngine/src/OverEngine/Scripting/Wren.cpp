#include "pcheader.h"
#include "Wren.h"

namespace WrenSources
{
	#include "Wren/entity.wren.inc"
	#include "Wren/input.wren.inc"
	#include "Wren/keycodes.wren.inc"
	#include "Wren/lib.wren.inc"
	#include "Wren/math.wren.inc"
	#include "Wren/script.wren.inc"
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
		wrenpp::VM::writeFn = [](const char* message) { if (strcmp(message, "\n")) OE_INFO("[wren] {}", message); };
		wrenpp::VM::errorFn = [](WrenErrorType type, const char* moduleName, int line, const char* message) -> void {
			const char* typeStr = ErrorTypeToString(type);
			if (moduleName)
				OE_ERROR("{} in {}:{}> {}", typeStr, moduleName, line, message);
			else
				OE_ERROR("{}> {}", typeStr, message);
		};


		wrenpp::VM::loadModuleFn = [](const char* mod) -> char* {
			#define WREN_MOD(m) if (strcmp(mod, #m) == 0) return const_cast<char*>(WrenSources::m##ModuleSource);

			WREN_MOD(entity)
			WREN_MOD(input)
			WREN_MOD(keycodes)
			WREN_MOD(lib)
			WREN_MOD(math)
			WREN_MOD(script)
			
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
	}

	WrenScriptClass::WrenScriptClass(const Ref<Wren>& vm, const char* moduleName, const char* className)
		: m_VM(vm)
	{
		wrenEnsureSlots(m_VM->GetRaw(), 1);
		wrenGetVariable(m_VM->GetRaw(), moduleName, className, 0);
		m_ClassHandle = wrenGetSlotHandle(m_VM->GetRaw(), 0);
		m_ConstructorHandle = wrenMakeCallHandle(m_VM->GetRaw(), "new(_)");
	}

	WrenScriptClass::~WrenScriptClass()
	{
		wrenReleaseHandle(m_VM->GetRaw(), m_ClassHandle);
		wrenReleaseHandle(m_VM->GetRaw(), m_ConstructorHandle);
	}

	Ref<WrenScriptInstance> WrenScriptClass::Construct(const Entity& entity) const
	{
		m_VM->CallMethod(m_ClassHandle, m_ConstructorHandle, entity);
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

	// TODO: Return result
	void WrenScriptInstance::OnCreate() const
	{
		m_VM->CallMethod(m_InstanceHandle, m_VM->GetOnCreateMethod());
	}

	void WrenScriptInstance::OnDestroy() const
	{
		m_VM->CallMethod(m_InstanceHandle, m_VM->GetOnDestroyMethod());
	}

	void WrenScriptInstance::OnUpdate(float delta) const
	{
		m_VM->CallMethod(m_InstanceHandle, m_VM->GetOnUpdateMethod(), delta);
	}

	void WrenScriptInstance::OnLateUpdate(float delta) const
	{
		m_VM->CallMethod(m_InstanceHandle, m_VM->GetOnLateUpdateMethod(), delta);
	}

	void WrenScriptInstance::OnCollisionEnter() const
	{
		m_VM->CallMethod(m_InstanceHandle, m_VM->GetOnCollisionEnterMethod());
	}

	void WrenScriptInstance::OnCollisionExit() const
	{
		m_VM->CallMethod(m_InstanceHandle, m_VM->GetOnCollisionExitMethod());
	}
}
