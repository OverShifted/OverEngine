#pragma once

// For type aliases
#include <memory>
#include <string>

#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

// For Assertion
#include "OverEngine/Core/Log.h"

// For mathematical stuff
#include "OverEngine/Core/Math/Math.h"

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
	#define OE_PLATFORM_LINUX
	#error "Android is not supported!"
#elif defined(__linux__)
	#define OE_PLATFORM_LINUX
#else
	/* Unknown compiler/platform */
	#error "Unknown platform!"
#endif
#if defined(OE_PLATFORM_LINUX) || defined(OE_PLATFORM_MACOS)
	#define OE_PLATFORM_UNIX_BASE
	#define LUA_USE_POSIX
#endif // End of platform detection

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
	#define _OE_ASSERT_TXT "========== Assertion Failed =========="

	#define OE_ASSERT(x, ...)                               \
	{                                                       \
		if(!(x))                                            \
		{                                                   \
			OE_ERROR(_OE_ASSERT_TXT);                       \
			OE_ERROR(__VA_ARGS__);                          \
			OE_ERROR(_OE_ASSERT_TXT); OE_DEBUGBREAK;        \
		}                                                   \
	}

	#define OE_CORE_ASSERT(x, ...)                          \
	{                                                       \
		if(!(x))                                            \
		{                                                   \
			OE_CORE_ERROR(_OE_ASSERT_TXT);                  \
			OE_CORE_ERROR(__VA_ARGS__);                     \
			OE_CORE_ERROR(_OE_ASSERT_TXT); OE_DEBUGBREAK;   \
		}                                                   \
	}
#else
	#define OE_ASSERT(x, ...)        { if (!(x)) OE_THROW(__VA_ARGS__); }
	#define OE_CORE_ASSERT(x, ...)   { if (!(x)) OE_THROW(__VA_ARGS__); }
#endif

#if !defined(_MSC_VER)
	#define sprintf_s snprintf
	#define sscanf_s sscanf
	#define strcpy_s(dest, destsz, src) strncpy(dest, src, destsz)
	#define strcat_s(dest, destsz, src) strncat(dest, src, destsz)
#endif


/// Constants
#define OE_IMGUI_BASE_TREE_VIEW_FLAGS           \
	(ImGuiTreeNodeFlags_OpenOnArrow |           \
	 ImGuiTreeNodeFlags_OpenOnDoubleClick |     \
	 ImGuiTreeNodeFlags_SpanAvailWidth |        \
	 ImGuiTreeNodeFlags_SpanFullWidth)

/// Operators
#define BIT(x) (1 << x)
#define OE_ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))
#define STD_CONTAINER_FIND(cont, val) (std::find(cont.begin(), cont.end(), val))
#define STD_CONTAINER_HAS(cont, val) (std::find(cont.begin(), cont.end(), val) != cont.end())
#define STD_MAP_HAS(cont, val) (cont.find(val) != cont.end())

/// Casts / Converters
#define IMVEC2_2_VECTOR2(imvec2) (::OverEngine::Vector2(imvec2.x, imvec2.y))

/// Other
#define BIND_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define STD_FUTURE_IS_DONE(f) (f.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
#define OE_SIMPLE_FOR_LOOP(n) for (uint32_t i##__LINE__ = 0; i##__LINE__ < n; i##__LINE__ += 1)


namespace OverEngine
{
	// Calculates offset of a member from pointer to member
	template<typename T, typename U>
	uint32_t OffsetOf(U T::*member)
	{
		return (uint32_t)((char*)&((T*)nullptr->*member) - (char*)nullptr);
	}

	// See https://stackoverflow.com/questions/45447361/how-to-move-certain-elements-of-stdvector-to-a-new-index-within-the-vector
	template<typename T>
	void Move(std::vector<T>& v, size_t from, size_t to)
	{
		if (from > to)
			std::rotate(v.rend() - from - 1, v.rend() - from, v.rend() - to);
		else
			std::rotate(v.begin() + from, v.begin() + from + 1, v.begin() + to + 1);
	}

	// See https://stackoverflow.com/questions/24263259/c-stdseterase-with-stdremove-if
	// And https://stackoverflow.com/a/24263441
	template <class T, class Comp, class Alloc, class Predicate>
	void DiscardIf(std::unordered_set<T, Comp, Alloc>& c, Predicate pred)
	{
		for (auto it{ c.begin() }, end{ c.end() }; it != end;)
		{
			if (pred(*it))
				it = c.erase(it);
			else
				++it;
		}
	}

	template<typename T> using Scope = std::unique_ptr<T>;
	template<typename T> using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	using String = std::string;

	template<typename T> using Vector = std::vector<T>;

	template<typename T, typename U> using UnorderedMap = std::unordered_map<T, U>;
	template<typename T, typename U> using Map = std::map<T, U>;

	template<typename T>
	struct Vec2T
	{
		union { T x, u, s; };
		union { T y, v, t; };
	};
}
