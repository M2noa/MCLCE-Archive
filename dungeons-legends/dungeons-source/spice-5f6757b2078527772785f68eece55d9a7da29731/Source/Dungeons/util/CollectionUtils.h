/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

namespace Util {
	struct PairKeyFunctor {
		template<typename T>
		typename T::first_type operator()(T& pair) const {
			return pair.first;
		}

	};

	struct PairValueFunctor {
		template<typename T>
		typename T::second_type operator()(T& pair) const {
			return pair.second;
		}

	};
}