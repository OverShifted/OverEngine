#pragma once

#if defined OE_PLATFORM_WINDOWS
	#if defined OE_BUILD_SHARED
		#if defined OE_PROJECT_BUILD_SHARED
			#define OVER_API __declspec(dllexport)
		#else
			#define OVER_API __declspec(dllimport)
		#endif // OE_BUILD_DYNAMIC
	#else
		#define OVER_API
	#endif // OE_BUILD_SHARED
#endif // OE_PLATFORM_WINDOWS

#if defined OE_PLATFORM_LINIUX
	#if defined OE_BUILD_SHARED
		#if defined OE_PROJECT_BUILD_SHARED
			#define OVER_API __attribute__((visibility("default")))
		#else
			#define OVER_API
		#endif // OE_BUILD_DYNAMIC
	#else
		#define OVER_API
	#endif // OE_BUILD_SHARED
#endif // OE_PLATFORM_LINIUX

#if defined OE_DEBUG

	// debugbreak
	#if defined OE_PLATFORM_WINDOWS
		#define OE_DEBUGBREAK() __debugbreak()
	#elif defined OE_PLATFORM_LINUX
		#include <signal.h>
		#define OE_DEBUGBREAK() raise(SIGTRAP)
	#endif // End of debugbreak

	#define OE_ENABLE_ASSERTS
#endif // OE_DEBUG

#if defined OE_ENABLE_ASSERTS
	#define OE_ASSERT(x, ...) { if(!(x)) { OE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define OE_CORE_ASSERT(x, ...) { if(!(x)) { OE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define OE_ASSERT(x, ...)
	#define OE_CORE_ASSERT(x, ...)
#endif


#define BIT(x) (1 << x)

#define OE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)