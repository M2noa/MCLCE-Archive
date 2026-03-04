#include "Random.h"

namespace Esteem
{

	std::mt19937 make_seeded_engine()
	{
		std::random_device r;
		std::seed_seq seed{ r(), r(), r(), r(), r(), r(), r(), r() };
		return std::mt19937(seed);
	}

	std::mt19937 Random::engine = make_seeded_engine();
}