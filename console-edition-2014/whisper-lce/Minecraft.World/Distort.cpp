#include "stdafx.h"
#include "Distort.h"

Distort::Distort(Synth *source, Synth *distort)
{
    this->source = source;
    this->distort = distort;
}

double Distort::getValue(double x, double y)
{
    return source->getValue(x + distort->getValue(x, y), y);
}
