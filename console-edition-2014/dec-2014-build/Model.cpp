#include "stdafx.h"
#include "TexOffs.h"
#include "Model.h"


Model::Model() 
{ 
	riding = false; 
	young=true; 
	texWidth=64; 
	texHeight=32;
}	

void Model::setMapTex(wstring id, int x, int y) 
{
	mappedTexOffs[id]=new TexOffs(x, y);
}

TexOffs *Model::getMapTex(wstring id) 
{
	// 4J-PB - assuming there will always be this one
	return mappedTexOffs[id];
}