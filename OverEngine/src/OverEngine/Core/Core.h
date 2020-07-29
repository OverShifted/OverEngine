#pragma once

#include <memory>
#include <vector>
#include <string>
#include "OverEngine/Core/Structs/List.h"
// #include "OverEngine/Core/Memory/Allocator.h"

// Platform detection using predefined macros
#ifdef _WIN32
	/* Windows x64/x86 */
	#ifdef _WIN64
		/* Windows x64  */
		#define OE_PLATFORM_WINDOWS
	#else
		/* Windows x86 */
		/* Maybe define some other macro for 32bit? */
		#define OE_PLATFORM_WINDOWS
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all the platforms
	 * so we must check all of them (in this order)
	 * to ensure that we're running on MAC
	 * and not some other Apple platform */
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define OE_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1
		#define OE_PLATFORM_MACOS
		#error "MacOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
	#define OE_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define OE_PLATFORM_LINUX
#else
	/* Unknown compiler/platform */
	#error "Unknown platform!"
#endif // End of platform detection

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
	#define OE_ENABLE_ASSERTS
	#define OE_DEBUGBREAK
#endif // OE_DEBUG

#ifdef OE_ENABLE_ASSERTS
	#define OE_ASSERT(x, ...) { if(!(x)) { OE_ERROR("========== Assertion Failed =========="); OE_ERROR(__VA_ARGS__); OE_ERROR("======================================"); OE_DEBUGBREAK; } }
	#define OE_CORE_ASSERT(x, ...) { if(!(x)) { OE_CORE_ERROR("========== Assertion Failed =========="); OE_CORE_ERROR(__VA_ARGS__); OE_CORE_ERROR("======================================"); OE_DEBUGBREAK; } }
#else
	#define OE_ASSERT( ... )
	#define OE_CORE_ASSERT( ... )
#endif

#define BIT(x) (1 << x)

#define OE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

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
	//using Vector = std::vector<T, OverEngine::Allocator<T>>;

	// UnorderedMap /////////////////////////////////////////////////////////////
	template<typename T, typename U>
	using UnorderedMap = std::unordered_map<T, U>;

	// String ///////////////////////////////////////////////////////////////////
	using String = std::string;

	// List /////////////////////////////////////////////////////////////////////
	template<typename T>
	using List = Structs::List<T>;
}
