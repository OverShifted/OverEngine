#include "pcheader.h"
#include "Runtime.h"

namespace OverEngine
{
	RuntimeType Runtime::s_Type;

	void Runtime::Init(RuntimeType type)
	{
		s_Type = type;
	}

	const RuntimeType& Runtime::GetType()
	{
		return s_Type;
	}

	void Runtime::HandleException(Exception& exception)
	{
		OE_CORE_ERROR(exception.What());
	}
}
