#include "stdafx.h"
#include "Synth.h"

doubleArray Synth::create(int width, int height)
{
    doubleArray result = doubleArray(width * height);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            result[x + y * width] = getValue(x, y);
        }
    }
    return result;
}
