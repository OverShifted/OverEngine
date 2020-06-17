#pragma once

#include "TimeStep.h"

namespace OverEngine
{
	class Time
	{
	protected:
		Time() = default;
	public:
		Time(const Time&) = delete;
		Time& operator=(const Time&) = delete;
	public:
		inline static float  GetTime() { return s_Instance->GetTimeImpl(); }
		inline static double GetTimeDouble() { return s_Instance->GetTimeDoubleImpl(); }

		inline static TimeStep GetDeltaTime() { return m_DeltaTime; }
		inline static void RecalculateDeltaTime() { float time = GetTime(); m_DeltaTime = time - m_LastFrameTime; m_LastFrameTime = time; }
	protected:
		virtual float GetTimeImpl() = 0;
		virtual double GetTimeDoubleImpl() = 0;
	private:
		static Time* s_Instance;

		static float m_LastFrameTime;
		static TimeStep m_DeltaTime;
	};
}