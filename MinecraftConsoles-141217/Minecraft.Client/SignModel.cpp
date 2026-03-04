#include "stdafx.h"
#include "SignModel.h"
#include "ModelPart.h"

SignModel::SignModel()
{
    cube = new ModelPart(this, 0, 0);
    cube->addBox(-12, -14, -1, 24, 12, 2, 0);
        
    cube2 = new ModelPart(this, 0, 14);
    cube2->addBox(-1, -2, -1, 2, 14, 2, 0);

	// 4J added - compile now to avoid random performance hit first time cubes are rendered
	cube->compile(1.0f/16.0f);
    cube2->compile(1.0f/16.0f);
}

void SignModel::render(bool usecompiled)
{
	cube->render(1/16.0f,usecompiled);
	cube2->render(1/16.0f,usecompiled);
}