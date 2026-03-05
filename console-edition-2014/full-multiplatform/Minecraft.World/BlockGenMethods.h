#pragma once

#include "ArrayWithLength.h"

class BlockGenMethods
{
public:
	static void generateBox(Level *level, byteArray blocks, int sx, int sy, int sz, int ex, int ey, int ez, BYTE edge, BYTE filling);
	static void generateFrame(Level *level, byteArray blocks, int sx, int sy, int ex, int ey, int flatZ, int direction, BYTE edge, BYTE filling);
	static void generateDirectionLine(Level *level, byteArray blocks, int sx, int sy, int sz, int ex, int ey, int ez, int startDirection, int endDirection, BYTE block);
	static void generateLine(Level *level, byteArray blocks, int sx, int sy, int sz, int ex, int ey, int ez, BYTE block);
};