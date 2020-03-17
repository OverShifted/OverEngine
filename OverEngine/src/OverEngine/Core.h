#pragma once

#ifdef OE_PLATFORM_WINDOWS
	#ifdef OE_BUILD_DYNAMIC
		#define OVER_API __declspec(dllexport)
	#else
		#define OVER_API __declspec(dllimport)
	#endif // OE_BUILD_DYNAMIC
#endif // OE_PLATFORM_WINDOWS
