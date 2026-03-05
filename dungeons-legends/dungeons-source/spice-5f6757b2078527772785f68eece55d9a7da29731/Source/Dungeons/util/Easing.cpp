/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

// Copyright (c) 2011 Bob Berkebile (pixelplacment)
// Please direct any bugs/comments/suggestions to http://pixelplacement.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// TERMS OF USE - EASING EQUATIONS
// Open source under the BSD License.
// Copyright (c)2001 Robert Penner
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// Redistributions of source code must retain the above copyright notice, this list of conditions
// and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice, this list of conditions
// and the following disclaimer in the documentation and/or other materials provided with the distribution.
// Neither the name of the author nor the names of contributors may be used to endorse or
// promote products derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS,
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "Dungeons.h"

#include "Easing.h"
#include <math.h>

std::vector<EaseFunc> Easing::mEasingFuncs = []() -> std::vector<EaseFunc> {
	
	std::vector<EaseFunc> easings;
	easings.resize((int)EasingType::_Count);
	
	easings[EasingType::Linear] = ([](float start, float end, float value) -> float {
		return Math::lerp(start, end, value);
	});

	easings[EasingType::Spring] = ([](float start, float end, float value) -> float {
		value = Math::clamp(value, 0.0f, 1.0f);
		value = (Math::sin(value * PI * (0.2f + 2.5f * value * value * value)) * std::pow(1.0f - value, 2.2f) + value) * (1.0f + (1.2f * (1.0f - value)));
		return start + (end - start) * value;
	});

	// Quad
	easings[EasingType::InQuad] = ([](float start, float end, float value) -> float {
		end -= start;
		return end * value * value + start;
	});

	easings[EasingType::OutQuad] = ([](float start, float end, float value) -> float {
		end -= start;
		return -end * value * (value - 2) + start;
	});

	easings[EasingType::InOutQuad] = ([](float start, float end, float value) -> float {
		value /= .5f;
		end -= start;
		if (value < 1) {
			return end * 0.5f * value * value + start;
		}
		value--;
		return -end * 0.5f * (value * (value - 2) - 1) + start;
	});
	
	// Cubic
	easings[EasingType::InCubic] = ([](float start, float end, float value) -> float {
		end -= start;
		return end * value * value * value + start;
	});

	easings[EasingType::OutCubic] = ([](float start, float end, float value) -> float {
		value--;
		end -= start;
		return end * (value * value * value + 1) + start;
	});

	easings[EasingType::InOutCubic] = ([](float start, float end, float value) -> float {
		value /= .5f;
		end -= start;
		if (value < 1) {
			return end * 0.5f * value * value * value + start;
		}
		value -= 2;
		return end * 0.5f * (value * value * value + 2) + start;
	});
	
	// Quart
	easings[EasingType::InQuart] = ([](float start, float end, float value) -> float {
		end -= start;
		return end * value * value * value * value + start;
	});

	easings[EasingType::OutQuart] = ([](float start, float end, float value) -> float {
		value--;
		end -= start;
		return -end * (value * value * value * value - 1) + start;
	});

	easings[EasingType::InOutQuart] = ([](float start, float end, float value) -> float {
		value /= .5f;
		end -= start;
		if (value < 1) {
			return end * 0.5f * value * value * value * value + start;
		}
		value -= 2;
		return -end * 0.5f * (value * value * value * value - 2) + start;
	});
	
	// Quint
	easings[EasingType::InQuint] = ([](float start, float end, float value) -> float {
		end -= start;
		return end * value * value * value * value * value + start;
	});

	easings[EasingType::OutQuint] = ([](float start, float end, float value) -> float {
		value--;
		end -= start;
		return end * (value * value * value * value * value + 1) + start;
	});

	easings[EasingType::InOutQuint] = ([](float start, float end, float value) -> float {
		value /= .5f;
		end -= start;
		if (value < 1) {
			return end * 0.5f * value * value * value * value * value + start;
		}
		value -= 2;
		return end * 0.5f * (value * value * value * value * value + 2) + start;
	});
	
	// Sine
	easings[EasingType::InSine] = ([](float start, float end, float value) -> float {
		end -= start;
		return -end * Math::cos(value * (PI * 0.5f)) + end + start;
	});

	easings[EasingType::OutSine] = ([](float start, float end, float value) -> float {
		end -= start;
		return end * Math::sin(value * (PI * 0.5f)) + start;
	});

	easings[EasingType::InOutSine] = ([](float start, float end, float value) -> float {
		end -= start;
		return -end * 0.5f * (Math::cos(PI * value) - 1) + start;
	});
	
	// Exponential
	easings[EasingType::InExpo] = ([](float start, float end, float value) -> float {
		end -= start;
		return end * std::pow(2.f, 10 * (value - 1)) + start;
	});

	easings[EasingType::OutExpo] = ([](float start, float end, float value) -> float {
		end -= start;
		return end * (-std::pow(2.f, -10 * value) + 1) + start;
	});

	easings[EasingType::InOutExpo] = ([](float start, float end, float value) -> float {
		value /= .5f;
		end -= start;
		if (value < 1) {
			return end * 0.5f * std::pow(2.f, 10 * (value - 1)) + start;
		}
		value--;
		return end * 0.5f * (-std::pow(2.f, -10 * value) + 2) + start;
	});
	
	// Circ
	easings[EasingType::InCirc] = ([](float start, float end, float value) -> float {
		end -= start;
		return -end * (Math::sqrt(1 - value * value) - 1) + start;
	});

	easings[EasingType::OutCirc] = ([](float start, float end, float value) -> float {
		value--;
		end -= start;
		return end * Math::sqrt(1 - value * value) + start;
	});
	easings[EasingType::InOutCirc] = ([](float start, float end, float value) -> float {
		value /= .5f;
		end -= start;
		if (value < 1) {
			return -end * 0.5f * (Math::sqrt(1 - value * value) - 1) + start;
		}
		value -= 2;
		return end * 0.5f * (Math::sqrt(1 - value * value) + 1) + start;
	});
	
	// Bounce	
	auto easeOutBounce = [](float start, float end, float value) -> float {
		value /= 1.f;
		end -= start;
		if (value < (1 / 2.75f)) {
			return end * (7.5625f * value * value) + start;
		}
		else if (value < (2 / 2.75f)) {
			value -= (1.5f / 2.75f);
			return end * (7.5625f * (value)* value + .75f) + start;
		}
		else if (value < (2.5 / 2.75)) {
			value -= (2.25f / 2.75f);
			return end * (7.5625f * (value)* value + .9375f) + start;
		}
		else {
			value -= (2.625f / 2.75f);
			return end * (7.5625f * (value)* value + .984375f) + start;
		}
	};
	
	auto easeInBounce = [easeOutBounce](float start, float end, float value) -> float {
		end -= start;
		float d = 1.f;
		return end - easeOutBounce(0, end, d - value) + start;
	};
		
	easings[EasingType::InBounce] = easeInBounce;
	easings[EasingType::OutBounce] = easeOutBounce;
	
	easings[EasingType::InOutBounce] = ([easeInBounce, easeOutBounce](float start, float end, float value) -> float {
		end -= start;
		float d = 1.f;
		if (value < d* 0.5f) {
			return easeInBounce(0, end, value * 2) * 0.5f + start;
		}
		else {
			return easeOutBounce(0, end, value * 2 - d) * 0.5f + end*0.5f + start;
		}
	});
	
	// Back
	easings[EasingType::InBack] = ([](float start, float end, float value) -> float {
		end -= start;
		value /= 1;
		float s = 1.70158f;
		return end * (value)* value * ((s + 1) * value - s) + start;
	});

	easings[EasingType::OutBack] = ([](float start, float end, float value) -> float {
		float s = 1.70158f;
		end -= start;
		value = (value)-1;
		return end * ((value)* value * ((s + 1) * value + s) + 1) + start;
	});
	
	easings[EasingType::InOutBack] = ([](float start, float end, float value) -> float {
		float s = 1.70158f;
		end -= start;
		value /= .5f;
		if ((value) < 1) {
			s *= (1.525f);
			return end * 0.5f * (value * value * (((s)+1) * value - s)) + start;
		}
		value -= 2;
		s *= (1.525f);
		return end * 0.5f * ((value)* value * (((s)+1) * value + s) + 2) + start;
	});
	
	// Elastic
	easings[EasingType::InElastic] = ([](float start, float end, float value) -> float {
		end -= start;

		float d = 1.0f;
		float p = d * .3f;
		float s = 0;
		float a = 0;

		if (value == 0) {
			return start;
		}

		if ((value /= d) == 1) {
			return start + end;
		}

		if (a == 0.0f || a < std::abs(end)) {
			a = end;
			s = p / 4;
		}
		else {
			s = p / (2 * PI) * (float)std::asin(end / a);
		}

		value--;
		return -(a * std::pow(2.f, 10 * value) * Math::sin((value * d - s) * (2 * PI) / p)) + start;
	});

	easings[EasingType::OutElastic] = ([](float start, float end, float value) -> float {
		end -= start;

		float d = 1.f;
		float p = d * .3f;
		float s = 0;
		float a = 0;

		if (value == 0) {
			return start;
		}

		if ((value /= d) == 1) {
			return start + end;
		}

		if (a == 0.f || a < std::abs(end)) {
			a = end;
			s = p * 0.25f;
		}
		else {
			s = p / (2 * PI) * std::asin(end / a);
		}

		return (a * std::pow(2.f, -10 * value) * Math::sin((value * d - s) * (2 * PI) / p) + end + start);
	});

	easings[EasingType::InOutElastic] = ([](float start, float end, float value) -> float {
		end -= start;

		float d = 1.f;
		float p = d * .3f;
		float s = 0;
		float a = 0;

		if (value == 0) {
			return start;
		}

		if ((value /= d*0.5f) == 2) {
			return start + end;
		}

		if (a == 0.f || a < std::abs(end)) {
			a = end;
			s = p / 4;
		}
		else {
			s = p / (2 * PI) * std::asin(end / a);
		}

		if (value < 1) {
			value--;
			return -0.5f * (a * std::pow(2.f, 10 * value) * Math::sin((value * d - s) * (2 * PI) / p)) + start;
		}
		
		value--;
		return a * std::pow(2.f, -10 * value) * Math::sin((value * d - s) * (2 * PI) / p) * 0.5f + end + start;
	});
	
	return easings;
}();

EaseFunc Easing::getEasingFunc(EasingType easingType) {
	return mEasingFuncs[easingType];
}