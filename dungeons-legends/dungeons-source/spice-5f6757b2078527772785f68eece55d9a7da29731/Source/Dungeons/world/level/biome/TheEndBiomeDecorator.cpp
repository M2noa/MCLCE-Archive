#include "Dungeons.h"

#include "world/level/biome/TheEndBiomeDecorator.h"

#include "util/Random.h"
#include "world/level/biome/Biome.h"
#include "world/level/BlockPos.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"

void TheEndBiomeDecorator::decorate(BlockSource* source, Random& random, Biome* biome, const BlockPos& origin, bool legacy, float limit) {
	decorateOres(source, random, origin);

	auto spikes = getSpikesForLevel(source->getLevel());
	for (auto&& spike : spikes) {
		if (spike.startsInChunk(origin)) {
			SpikeFeature spikeFeature(spike);
			spikeFeature.place(*source, BlockPos(spike.getCenterX(), 45, spike.getCenterZ()), random);
		}
	}
}

std::vector<SpikeFeature::EndSpike> TheEndBiomeDecorator::getSpikesForLevel(Level& level) {
	Random random(level.getSeed());
	return _loadSpikes(random);
}

std::vector<SpikeFeature::EndSpike> TheEndBiomeDecorator::_loadSpikes(Random& random) {
	const int numberOfSpikes = 10;
	const int spikeDistance = 42;

	// Generate all sizes and use them randomly.
	std::vector<int> sizes(numberOfSpikes);
	for (int i = 0; i < numberOfSpikes; ++i) {
		sizes[i] = i;
	}
	std::random_shuffle(sizes.begin(), sizes.end(), random);

	// Generate all spike features.
	std::vector<SpikeFeature::EndSpike> spikes;

	for (int i = 0; i < numberOfSpikes; ++i) {
		int x = static_cast<int>(spikeDistance * Math::cos(2.0f * (-PI + (PI / numberOfSpikes) * i)));
		int z = static_cast<int>(spikeDistance * Math::sin(2.0f * (-PI + (PI / numberOfSpikes) * i)));
		int size = sizes[i];
		int radius = 2 + (size / 3);
		int height = 64 + 12 + (size * 3);
		bool guarded = size == 1 || size == 2;
		spikes.emplace_back(x, z, radius, height, guarded);
	}
	
	return spikes;
}
