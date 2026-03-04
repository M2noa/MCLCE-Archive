#include "stdafx.h"
#include "SmoothFloat.h"

SmoothFloat::SmoothFloat()
{
	targetValue = 0.0f;
	remainingValue = 0.0f;
	lastAmount = 0.0f;
}

float SmoothFloat::getNewDeltaValue(float deltaValue, float accelerationAmount)
{
    targetValue += deltaValue;

    deltaValue = (targetValue - remainingValue) * accelerationAmount;
    lastAmount = lastAmount + (deltaValue - lastAmount) * 0.5f;
    if ((deltaValue > 0 && deltaValue > lastAmount) || (deltaValue < 0 && deltaValue < lastAmount))
	{
        deltaValue = lastAmount;
    }
    remainingValue += deltaValue;

    return deltaValue;
}

float SmoothFloat::getTargetValue()
{
	return targetValue;
}