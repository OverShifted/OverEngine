#pragma once

#include "Core.h"
#include "OverEngine/Events/Event.h"


namespace OverEngine
{
	class OVER_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in client
	Application* CreateApplication();
}