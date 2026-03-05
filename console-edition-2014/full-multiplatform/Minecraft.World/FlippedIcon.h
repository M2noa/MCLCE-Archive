#pragma once
using namespace std;

#include "Icon.h"

class FlippedIcon : public Icon
{
private:
	Icon *base;
	const bool horizontal;
	const bool vertical;

public:
	FlippedIcon(Icon *base, bool horizontal, bool vertical);

	int getX() const;
	int getY() const;
	int getWidth() const;
	int getHeight() const;
	float getU0(bool adjust = false) const;
	float getU1(bool adjust = false) const;
	float getU(double offset, bool adjust = false) const;
	float getV0(bool adjust = false) const;
	float getV1(bool adjust = false) const;
	float getV(double offset, bool adjust = false) const;
	wstring getName() const;
	int getSourceWidth() const;
	int getSourceHeight() const;
	int getFlags() const;			// 4J added
	void setFlags(int flags);		// 4J added
};