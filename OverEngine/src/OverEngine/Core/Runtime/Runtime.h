#pragma once

#include <fmt/format.h>
#include <exception>

namespace OverEngine
{
	enum class RuntimeType
	{
		Editor, Player
	};

	class Exception;
	class Runtime
	{
	public:
		static void Init(RuntimeType type);
		static const RuntimeType& GetType();

		static void HandleException(const Exception& exception);
	private:
		static RuntimeType s_Type;
	};

	// Base of all OverEngine exceptions
	class Exception : public std::runtime_error
	{
	public:
		Exception(const String& message) : std::runtime_error(message.c_str()) {}
		Exception(const char* message) : std::runtime_error(message) {}
	};

	#define OE_THROW(...) (::OverEngine::Runtime::HandleException(::OverEngine::Exception(fmt::format(__VA_ARGS__))))
}
