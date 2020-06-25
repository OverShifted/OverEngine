#include "pcheader.h"
#include "Init.h"

#include "OverEngine/Core/Log.h"

namespace OverEngine
{
	void Initialize()
	{
		Log::Init();

		#ifdef _MSC_VER
			OE_CORE_INFO("OverEngine v0.0 [MSC {0}]", _MSC_VER);
		#else
			OE_CORE_INFO("OverEngine v0.0");
		#endif
	}
}