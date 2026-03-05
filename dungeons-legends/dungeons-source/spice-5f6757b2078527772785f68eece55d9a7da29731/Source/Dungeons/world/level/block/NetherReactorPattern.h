/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

class NetherReactorPattern {
public:
	NetherReactorPattern();

	unsigned int getBlockAt(int level, int x, int z);

private:
	void setBlockAt(int level, int x, int z, int block);

	unsigned int pattern[3][3][3];
};
