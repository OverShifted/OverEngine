#include "pcheader.h"
#include "Random.h"

namespace OverEngine
{
	std::mt19937 Random::s_RandomEngineUInt32;
	std::uniform_int_distribution<std::mt19937::result_type> Random::s_DistributionUInt32;

	std::mt19937_64 Random::s_RandomEngineUInt64;
	std::uniform_int_distribution<std::mt19937_64::result_type> Random::s_DistributionUInt64;
}
