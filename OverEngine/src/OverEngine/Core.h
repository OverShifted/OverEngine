#pragma once

#ifdef OE_PLATFORM_WINDOWS
	#ifdef OE_BUILD_SHARED
		#define OVER_API __declspec(dllexport)
	#else
		#define OVER_API __declspec(dllimport)
	#endif // OE_BUILD_DYNAMIC
#endif // OE_PLATFORM_WINDOWS

#ifdef OE_ENABLE_ASSERTS
	#define OE_ASSERT(x, ...) { if(!(x)) { OE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define OE_CORE_ASSERT(x, ...) { if(!(x)) { OE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define OE_ASSERT(x, ...)
	#define OE_CORE_ASSERT(x, ...)
#endif


#define BIT(x) (1 << x)