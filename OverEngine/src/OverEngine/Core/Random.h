#pragma once

#include <random>

namespace OverEngine
{
	class Random
	{
	public:
		static void Init()
		{
			s_RandomEngine.seed(std::random_device()());
			s_RandomEngine64.seed(std::random_device()());
		}

		static float Float()
		{
			return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
		}

		static float Range(float min, float max)
		{
			return Float() * (max - min) + min;
		}

		static uint32_t UInt32()
		{
			return s_Distribution(s_RandomEngine);
		}

		static uint64_t UInt64()
		{
			return s_Distribution64(s_RandomEngine64);
		}
	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;

		static std::mt19937_64 s_RandomEngine64;
		static std::uniform_int_distribution<std::mt19937_64::result_type> s_Distribution64;
	};
}
