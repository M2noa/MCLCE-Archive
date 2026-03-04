#pragma once

#include "Model.h"

class Cube;

class SignModel : public Model
{
public:
	using Model::render;
	ModelPart *cube;
    ModelPart *cube2;
    
    SignModel();
    void render(bool usecompiled);
};
