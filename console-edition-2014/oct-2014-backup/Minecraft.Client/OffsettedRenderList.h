#pragma once

class IntBuffer;

class OffsettedRenderList
{
private:
	int x, y, z;
    float xOff, yOff, zOff;
    IntBuffer *lists;
    bool inited;
    bool rendered ;

public:
	OffsettedRenderList();	 // 4J added
    void init(int x, int y, int z, double xOff, double yOff, double zOff);
    bool isAt(int x, int y, int z);
    void add(int list);
    void render();
    void clear();
};