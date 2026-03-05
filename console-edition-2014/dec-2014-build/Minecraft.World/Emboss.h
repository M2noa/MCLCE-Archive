#pragma once
#include "Synth.h"

class Emboss: public Synth
{
private:
    Synth *synth;

public:
    Emboss(Synth *synth);

    virtual double getValue(double x, double y);
};