#include "pcheader.h"

#include "LinuxTime.h"

#include <GLFW/glfw3.h>

namespace OverEngine
{
	Time* Time::s_Instance = new LinuxTime();

	float LinuxTime::GetTimeImpl()
	{
		return (float)glfwGetTime();
	}

	double LinuxTime::GetTimeDoubleImpl()
	{
		return glfwGetTime();
	}

}
