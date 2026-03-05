/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/dimension/Dimension.h"

class HellDimension : public Dimension {

public:

	HellDimension(Level& level);
	virtual ~HellDimension();

	virtual void init() override;

	virtual void updateLightRamp() override;

	virtual bool isNaturalDimension() const override;
	virtual bool isValidSpawn(int x, int z) const override;

	virtual bool showSky() const override;
	virtual float getTimeOfDay(int time, float a) const override;
	virtual Color getFogColor(float br) const override;
	virtual float getFogDistanceScale() const override;
	virtual bool isFoggyAt(int x, int z) const override;

	virtual bool mayRespawn() const override;

	virtual std::string getName() const override;

	virtual Vec3 translatePosAcrossDimension(const Vec3& originalPos, DimensionId fromId) const override;
};
