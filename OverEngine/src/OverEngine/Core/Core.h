#pragma once

// For type aliases
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>

// For OE(_CORE)_ASSERT
#include "OverEngine/Core/Log.h"
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
 * since android is based on the Linux kernel
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
#define ASSERT_FAILED_START_TXT "========== Assertion Failed =========="
#define ASSERT_FAILED_END_TXT "========== Assertion Failed =========="
#define OE_ASSERT(x, ...) { if(!(x)) { OE_ERROR(ASSERT_FAILED_START_TXT); OE_ERROR(__VA_ARGS__); OE_ERROR(ASSERT_FAILED_END_TXT); OE_DEBUGBREAK; } }
#define OE_CORE_ASSERT(x, ...) { if(!(x)) { OE_CORE_ERROR(ASSERT_FAILED_START_TXT); OE_CORE_ERROR(__VA_ARGS__); OE_CORE_ERROR(ASSERT_FAILED_END_TXT); OE_DEBUGBREAK; } }
#else
	#define OE_ASSERT( ... )
	#define OE_CORE_ASSERT( ... )
#endif

#if !defined(_MSC_VER)
	#define sprintf_s(a, b, c, ...) sprintf(a, c, __VA_ARGS__)
	#define strcpy_s(a, b, c) strcpy(a, c)
	#define strcat_s(a, b, c) strcat(a, c)
	#define sscanf_s sscanf
#endif

#define BIT(x) (1 << x)

#define BIND_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define NULL_REF(T) (*(T*)nullptr)

#define CAPITAL_A_ASCII 65
#define SMALL_A_ASCII 97
#define CHAR_1_ASCII 49
#define CHAR_0_ASCII 48

#define DIGIT_TO_CHAR(digit) ((char)(digit + CHAR_0_ASCII))

#define OE_IMGUI_BASE_TREE_VIEW_FLAGS (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth)

#define OE_ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

#define IMVEC2_2_VECTOR2(imvec2) (::OverEngine::Vector2(imvec2.x, imvec2.y))
#define IMVEC4_2_VECTOR2(imvec4) (::OverEngine::Vector4(imvec4.x, imvec4.y, imvec4.z, imvec4.w))

namespace OverEngine
{
	// Calculates offset of a member from pointer to member
	template<typename T, typename U>
	uint32_t OffsetOf(U T::*member)
	{
		return (uint32_t)((char*)&((T*)nullptr->*member) - (char*)nullptr);
	}

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

	// String ///////////////////////////////////////////////////////////////////
	using String = std::string;

	// Vector ///////////////////////////////////////////////////////////////////
	template<typename T>
	using Vector = std::vector<T>;
	//using Vector = std::vector<T, OverEngine::Allocator<T>>;

	// UnorderedMap /////////////////////////////////////////////////////////////
	template<typename T, typename U>
	using UnorderedMap = std::unordered_map<T, U>;

	// Map //////////////////////////////////////////////////////////////////////
	template<typename T, typename U>
	using Map = std::map<T, U>;
}
