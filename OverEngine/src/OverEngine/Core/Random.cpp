#include "pcheader.h"
#include "Random.h"

namespace OverEngine
{
	std::mt19937 Random::s_RandomEngine;
	std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

	std::mt19937_64 Random::s_RandomEngine64;
	std::uniform_int_distribution<std::mt19937_64::result_type> Random::s_Distribution64;
}
