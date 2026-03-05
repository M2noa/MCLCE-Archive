/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

//
// Go here to see these in action: http://easings.net/
// The type of easing to use based on Robert Penner's open source easing equations (http://www.robertpenner.com/easing_terms_of_use.html).
//
enum EasingType {
	Linear,	
	Spring,

	InQuad,
	OutQuad,
	InOutQuad,

	InCubic,
	OutCubic,
	InOutCubic,
	
	InQuart,
	OutQuart,
	InOutQuart,
	
	InQuint,
	OutQuint,
	InOutQuint,
	
	InSine,
	OutSine,
	InOutSine,
	
	InExpo,
	OutExpo,
	InOutExpo,
	
	InCirc,
	OutCirc,
	InOutCirc,
			
	InBounce,
	OutBounce,
	InOutBounce,
	
	InBack,
	OutBack,
	InOutBack,
	
	InElastic,
	OutElastic,
	InOutElastic,
	
	_Count,
	_Invalid
};

using EaseFunc = std::function<float(float, float, float)>;

class Easing {
public:
	static EaseFunc getEasingFunc(EasingType easingType);
private:
	static std::vector<EaseFunc> mEasingFuncs;
};
