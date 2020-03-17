#pragma once

#include "Core.h"

namespace OverEngine
{
	class OVER_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};
}