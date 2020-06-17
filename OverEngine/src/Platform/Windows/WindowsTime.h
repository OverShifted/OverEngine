#pragma once

#include "OverEngine/Core/Time/Time.h"

namespace OverEngine
{
	class WindowsTime : public Time
	{
	protected:
		virtual float GetTimeImpl() override;
		virtual double GetTimeDoubleImpl() override;
	};
}