#pragma once
#include "Synth.h"

class Scale : public Synth
{
private:
	Synth *synth;
	double xScale;
	double yScale;

public:
    Scale(Synth *synth, double xScale, double yScale);

    virtual double getValue(double x, double y);
};