#include "pcheader.h"

#include "WindowsTime.h"

#include <GLFW/glfw3.h>

namespace OverEngine
{
	Time* Time::s_Instance = new WindowsTime();

	float WindowsTime::GetTimeImpl()
	{
		return (float)glfwGetTime();
	}

	double WindowsTime::GetTimeDoubleImpl()
	{
		return glfwGetTime();
	}

}
