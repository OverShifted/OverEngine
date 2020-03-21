#include "pcheader.h"
#include "Application.h"

#include "OverEngine/Events/ApplicationEvent.h"
#include "OverEngine/Log.h"

namespace OverEngine
{
	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent e(120, 403);
		OE_TRACE(e);
		while (true);
	}
}