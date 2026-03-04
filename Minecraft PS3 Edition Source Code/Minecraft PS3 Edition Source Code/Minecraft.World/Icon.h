#pragma once
using namespace std;


class Icon
{
public:
	static const int TYPE_TERRAIN = 0;
	static const int TYPE_ITEM = 1;

	static const int  IS_GRASS_SIDE = 1;
	static const int  IS_GRASS_TOP = 2;


	virtual int getX() const = 0;
	virtual int getY() const = 0;
	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
	virtual float getU0(bool adjust = false) const = 0;
	virtual float getU1(bool adjust = false) const = 0;
	virtual float getU(double offset, bool adjust = false) const = 0;
	virtual float getV0(bool adjust = false) const = 0;
	virtual float getV1(bool adjust = false) const = 0;
	virtual float getV(double offset, bool adjust = false) const = 0;
	virtual wstring getName() const = 0;
	virtual int getSourceWidth() const = 0;
	virtual int getSourceHeight() const = 0;
	virtual int getFlags() const = 0;			// 4J added
	virtual void setFlags(int flags) = 0;	// 4J added
};
