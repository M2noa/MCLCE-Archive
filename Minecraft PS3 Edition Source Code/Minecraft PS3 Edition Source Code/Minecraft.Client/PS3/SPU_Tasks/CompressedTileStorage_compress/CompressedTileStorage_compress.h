#pragma once

class CompressedTileStorage_compress_dataIn
{
public:
	unsigned char	*indicesAndData;
	int				allocatedSize;
	int				upgradeBlock;
	bool			neededCompressed;

	unsigned char	*newIndicesAndData;
	int				newAllocatedSize;
	int				padding[2];
};
