#include "stdafx.h"
#include "Rotate.h"

Rotate::Rotate(Synth *synth, float angle)
{
    this->synth = synth;

    _sin = sin(angle);
    _cos = cos(angle);
}

double Rotate::getValue(double x, double y)
{
	return synth->getValue(x * _cos + y * _sin, y * _cos - x * _sin);
}