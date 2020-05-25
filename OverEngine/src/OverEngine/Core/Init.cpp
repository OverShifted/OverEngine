#include "pcheader.h"

#include "Init.h"
#include "OverEngine/Input/InputSystem.h"

namespace OverEngine
{
	void Initialize()
	{
		OverEngine::    Log    ::Init();
		// OverEngine::InputSystem::Init();

		OE_CORE_INFO("OverEngine v0.0");
	}
}