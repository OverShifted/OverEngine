#pragma once

#include <fmt/format.h>

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

	class Exception
	{
	public:
		Exception(const String& message = String())
			: m_Message(message)
		{
		}

		virtual const char* What() const { return m_Message.empty() ? "Unknown Exception!" : m_Message.c_str(); }
	protected:
		String m_Message;
	};

	#define OE_THROW(...) (::OverEngine::Runtime::HandleException(Exception(fmt::format(__VA_ARGS__))))
}
