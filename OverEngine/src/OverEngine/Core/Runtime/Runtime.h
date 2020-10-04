#pragma once

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

		static void HandleException(Exception& exception);
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

		virtual const char* What() { return m_Message.empty() ? "Unknown Exception!" : m_Message.c_str(); }
	protected:
		String m_Message;
	};

	#define OE_THROW(exceptipn) (::OverEngine::Runtime::HandleException(exceptipn))
}
