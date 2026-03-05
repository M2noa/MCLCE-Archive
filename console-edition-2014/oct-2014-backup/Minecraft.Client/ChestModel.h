#pragma once

#include "Model.h"

class Cube;

class ChestModel : public Model
{
public:
	using Model::render;

	ModelPart *lid;
	ModelPart *bottom;
	ModelPart *lock;

	ChestModel();
	void render(bool usecompiled);
};
