#pragma once

#include <memory>
#include <vector>
#include <string>
#include "OverEngine/Core/Structs/List.h"

#if defined(OE_PLATFORM_LINUX) || defined(OE_PLATFORM_MACOS)
	#define OE_PLATFORM_UNIX_BASE
#endif

#ifdef OE_PLATFORM_WINDOWS
	#ifdef OE_BUILD_SHARED
		#ifdef OE_PROJECT_BUILD_SHARED
			#define OVER_API __declspec(dllexport)
		#else
			#define OVER_API __declspec(dllimport)
		#endif // OE_BUILD_DYNAMIC
	#else
		#define OVER_API
	#endif // OE_BUILD_SHARED
#endif // OE_PLATFORM_WINDOWS

#ifdef OE_PLATFORM_UNIX_BASE
	#ifdef OE_BUILD_SHARED
		#ifdef OE_PROJECT_BUILD_SHARED
			#define OVER_API __attribute__((visibility("default")))
		#else
			#define OVER_API
		#endif // OE_BUILD_DYNAMIC
	#else
		#define OVER_API
	#endif // OE_BUILD_SHARED
#endif // OE_PLATFORM_LINIUX

#ifdef OE_DEBUG

	// debugbreak
	#ifdef OE_PLATFORM_WINDOWS
		#define OE_DEBUGBREAK __debugbreak()
	#elif defined OE_PLATFORM_LINUX
		#include <signal.h>
		#define OE_DEBUGBREAK raise(SIGTRAP)
	#endif // End of debugbreak

	#define OE_ENABLE_ASSERTS
#else
	#define OE_DEBUGBREAK
#endif // OE_DEBUG

#ifdef OE_ENABLE_ASSERTS
	#define OE_ASSERT(x, ...) { if(!(x)) { OE_ERROR("Assertion Failed: {0}", __VA_ARGS__); OE_DEBUGBREAK; } }
	#define OE_CORE_ASSERT(x, ...) { if(!(x)) { OE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); OE_DEBUGBREAK; } }
#else
	#define OE_ASSERT( ... )
	#define OE_CORE_ASSERT( ... )
#endif

#define BIT(x) (1 << x)

#define OE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace OverEngine
{
	// Scope ////////////////////////////////////////////////////////////////////
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	// Ref //////////////////////////////////////////////////////////////////////
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	// Vector ///////////////////////////////////////////////////////////////////
	template<typename T>
	using Vector = std::vector<T>;

	// String ///////////////////////////////////////////////////////////////////
	using String = std::string;

	// List /////////////////////////////////////////////////////////////////////
	template<typename T>
	using List = Structs::List<T>;
}
