/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"
#include "world/level/GeneratorType.h"
#include "CommonTypes.h"

class LayerData {
public:

	int* mParentArea = mBuff0;
	int* mResult = mBuff1;
	void swap();

private:
	int mBuff0[24 * 24];
	int mBuff1[24 * 24];
};

class Layer;
typedef std::shared_ptr<Layer> LayerPtr;

class Layer : public std::enable_shared_from_this<Layer>{
protected:

	static const int OCEAN_ID = 0;
	static const int WARM_ID = 1;
	static const int MEDIUM_ID = 2;
	static const int COLD_ID = 3;
	static const int ICE_ID = 4;

	static const int SPECIAL_MASK = 0xf00;
	static const int SPECIAL_SHIFT = 8;
public:

	static void getDefaultLayers(int64_t seed, LayerPtr& biomeLayer, LayerPtr& zoomedLayer, GeneratorType type);

	Layer(RandomSeed seedMixup);
	virtual ~Layer();

	virtual void init(int64_t seed);
	void initRandom(int64_t x, int64_t y);
	int nextRandom(int max);
	virtual void fillArea(LayerData& layerData, int originX, int originY, int width, int height) = 0;

protected:

	static bool _isSame(const int a, const int b);
	static bool _isOcean(int id);
	int _random(int* list, int count);
	virtual int _modeOrRandom(const int a, const int b, const int c, const int d);

protected:

	int64_t mSeed = 0;
	int64_t mRval = 0;
	int64_t mSeedMixup = 0;
	LayerPtr mParent;
};
