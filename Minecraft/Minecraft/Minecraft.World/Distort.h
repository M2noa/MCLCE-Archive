#pragma once
#include "Synth.h"

class Distort: public Synth
{
private:
    Synth *source;
    Synth *distort;

public:
    Distort(Synth *source, Synth *distort);

    virtual double getValue(double x, double y);
};