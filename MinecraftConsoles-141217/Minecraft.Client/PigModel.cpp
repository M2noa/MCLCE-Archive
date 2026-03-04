#include "stdafx.h"
#include "PigModel.h"
#include "ModelPart.h"

PigModel::PigModel() : QuadrupedModel(6, 0)
{
	head->texOffs(16, 16)->addBox(-2.0f, 0.0f, -9.0f, 4, 3, 1, 0.0f);
	yHeadOffs = 4;

	head->compile(1.0f/16.0f);
}

PigModel::PigModel(float grow) : QuadrupedModel(6, grow)
{
	head->texOffs(16, 16)->addBox(-2.0f, 0.0f, -9.0f, 4, 3, 1, grow);
	yHeadOffs = 4;

	head->compile(1.0f/16.0f);
}

