/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/dimension/Dimension.h"

class Level;

class NormalDimension : public Dimension {

public:

	NormalDimension(Level& level);
	virtual ~NormalDimension();

	virtual void init() override;
	virtual std::string getName() const override;

	virtual Vec3 translatePosAcrossDimension(const Vec3& originalPos, DimensionId fromId) const override;
};
