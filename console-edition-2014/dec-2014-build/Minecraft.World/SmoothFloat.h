#pragma once

class SmoothFloat
{
private:
	float targetValue;
    float remainingValue;
    float lastAmount;
public:
	SmoothFloat();	// 4J added
	float getNewDeltaValue(float deltaValue, float accelerationAmount);
    float getTargetValue();
};