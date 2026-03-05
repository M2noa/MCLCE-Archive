#include "stdafx.h"
#include "Model.h"
#include "ModelPart.h"
#include "Cube.h"



// 4J - added - helper function to set up vertex arrays
VertexArray Cube::VertexArray4(Vertex *v0, Vertex *v1, Vertex *v2, Vertex *v3)
{
	VertexArray ret = VertexArray(4);
	ret[0] = v0;
	ret[1] = v1;
	ret[2] = v2;
	ret[3] = v3;

	return ret;
}

//void Cube::addBox(float x0, float y0, float z0, int w, int h, int d, float g)
Cube::Cube(ModelPart *modelPart, int xTexOffs, int yTexOffs, float x0, float y0, float z0, int w, int h, int d, float g, int faceMask /* = 63 */, bool bFlipPoly3UVs) :		// 4J - added faceMask, added bFlipPoly3UVs to reverse the uvs back so player skins display right
	x0(x0),
	y0(y0),
	z0(z0),
	x1(x0 + w),
	y1(y0 + h),
	z1(z0 + d)
{
// 	this->x0 = x0;
// 	this->y0 = y0;
// 	this->z0 = z0;
// 	this->x1 = x0 + w;
// 	this->y1 = y0 + h;
// 	this->z1 = z0 + d;

    vertices = VertexArray(8);
    polygons = PolygonArray(6);

    float x1 = x0 + w;
    float y1 = y0 + h;
    float z1 = z0 + d;

    x0 -= g;
    y0 -= g;
    z0 -= g;
    x1 += g;
    y1 += g;
    z1 += g;

    if (modelPart->bMirror)
	{
        float tmp = x1;
        x1 = x0;
        x0 = tmp;
    }

    Vertex *u0 = new Vertex(x0, y0, z0, 0, 0);
    Vertex *u1 = new Vertex(x1, y0, z0, 0, 8);
    Vertex *u2 = new Vertex(x1, y1, z0, 8, 8);
    Vertex *u3 = new Vertex(x0, y1, z0, 8, 0);
		   
    Vertex *l0 = new Vertex(x0, y0, z1, 0, 0);
    Vertex *l1 = new Vertex(x1, y0, z1, 0, 8);
    Vertex *l2 = new Vertex(x1, y1, z1, 8, 8);
    Vertex *l3 = new Vertex(x0, y1, z1, 8, 0);

    vertices[0] = u0;
    vertices[1] = u1;
    vertices[2] = u2;
    vertices[3] = u3;
    vertices[4] = l0;
    vertices[5] = l1;
    vertices[6] = l2;
    vertices[7] = l3;

	// 4J - added ability to mask individual faces
	int faceCount = 0;
    if( faceMask & 1 ) polygons[faceCount++] = new _Polygon(VertexArray4(l1, u1, u2, l2), xTexOffs + d + w, yTexOffs + d, xTexOffs + d + w + d, yTexOffs + d + h, modelPart->xTexSize, modelPart->yTexSize); // Right
    if( faceMask & 2 ) polygons[faceCount++] = new _Polygon(VertexArray4(u0, l0, l3, u3), xTexOffs + 0, yTexOffs + d, xTexOffs + d, yTexOffs + d + h, modelPart->xTexSize, modelPart->yTexSize); // Left
    if( faceMask & 4 ) polygons[faceCount++] = new _Polygon(VertexArray4(l1, l0, u0, u1), xTexOffs + d, yTexOffs + 0, xTexOffs + d + w, yTexOffs + d, modelPart->xTexSize, modelPart->yTexSize); // Up
	if(bFlipPoly3UVs)
	{
		if( faceMask & 8 ) polygons[faceCount++] = new _Polygon(VertexArray4(u2, u3, l3, l2), xTexOffs + d + w, yTexOffs + 0, xTexOffs + d + w + w, yTexOffs + d, modelPart->xTexSize, modelPart->yTexSize); // Down
	}
	else
	{
    if( faceMask & 8 ) polygons[faceCount++] = new _Polygon(VertexArray4(u2, u3, l3, l2), xTexOffs + d + w, yTexOffs + d, xTexOffs + d + w + w, yTexOffs + 0, modelPart->xTexSize, modelPart->yTexSize); // Down
	}
    if( faceMask & 16 ) polygons[faceCount++] = new _Polygon(VertexArray4(u1, u0, u3, u2), xTexOffs + d, yTexOffs + d, xTexOffs + d + w, yTexOffs + d + h, modelPart->xTexSize, modelPart->yTexSize); // Front
    if( faceMask & 32 ) polygons[faceCount++] = new _Polygon(VertexArray4(l0, l1, l2, l3), xTexOffs + d + w + d, yTexOffs + d, xTexOffs + d + w + d + w, yTexOffs + d + h, modelPart->xTexSize, modelPart->yTexSize); // Back
	polygons.length = faceCount;

    if (modelPart->bMirror)
	{
        for (unsigned int i = 0; i < polygons.length; i++)
            polygons[i]->mirror();
    }
}


void Cube::render(Tesselator *t,float scale)
{
	for (int i = 0; i < polygons.length; i++) 
	{
		polygons[i]->render(t, scale);
	}
}

Cube *Cube::setId(const wstring &id) 
{
	this->id = id;
	return this;
}
