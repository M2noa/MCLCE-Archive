#pragma once
#include "Synth.h"

class Rotate : public Synth
{
private:
	Synth *synth;
	double _sin;
	double _cos;

public:
    Rotate(Synth *synth, float angle);

    virtual double getValue(double x, double y);
};