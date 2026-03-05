#pragma once
#include "ZombieModel.h"

class SkeletonModel : public ZombieModel
{
private:
	void _init(float g);

public:
	SkeletonModel();
	SkeletonModel(float g);
	virtual void setupAnim(float time, float r, float bob, float yRot, float xRot, float scale, unsigned int uiBitmaskOverrideAnim=0);
};