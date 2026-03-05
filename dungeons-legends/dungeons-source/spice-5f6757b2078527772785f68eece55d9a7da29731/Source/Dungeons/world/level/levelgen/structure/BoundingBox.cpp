#include "Dungeons.h"

#include "world/level/levelgen/structure/BoundingBox.h"
#include "nbt/IntArrayTag.h"

Unique<IntArrayTag> BoundingBox::createTag() const {
	TagMemoryChunk mem;
	auto data = mem.alloc<int>(6);
	data[0] = x0;
	data[1] = y0;
	data[2] = z0;
	data[3] = x1;
	data[4] = y1;
	data[5] = z1;
	return make_unique<IntArrayTag>("", mem);
}
