#pragma once

#include <random>

namespace OverEngine
{
	class Random
	{
	public:
		static void Init()
		{
			s_RandomEngineUInt32.seed(std::random_device()());
			s_RandomEngineUInt64.seed(std::random_device()());
		}

		static float Float()
		{
			return (float)s_DistributionUInt32(s_RandomEngineUInt32) / (float)std::numeric_limits<std::mt19937::result_type>::max();
		}

		static float Range(float min, float max)
		{
			return Float() * (max - min) + min;
		}

		static uint32_t UInt32()
		{
			return s_DistributionUInt32(s_RandomEngineUInt32);
		}

		static uint64_t UInt64()
		{
			return s_DistributionUInt64(s_RandomEngineUInt64);
		}
	private:
		static std::mt19937 s_RandomEngineUInt32;
		static std::uniform_int_distribution<std::mt19937::result_type> s_DistributionUInt32;

		static std::mt19937_64 s_RandomEngineUInt64;
		static std::uniform_int_distribution<std::mt19937_64::result_type> s_DistributionUInt64;
	};
}
