#pragma once

/**
* @file    SimplexNoise.h
* @brief   A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D).
*
* Copyright (c) 2014-2018 Sebastien Rombauts (sebastien.rombauts@gmail.com)
*
* Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
* or copy at http://opensource.org/licenses/MIT)
*/

#include <cstddef>  // size_t

/**
* @brief A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D, 4D).
*/
template<class P, class I = int>
class SimplexNoise
{
private:
	// Parameters of Fractional Brownian Motion (fBm) : sum of N "octaves" of noise
	uint8_t perm[256];
	P mFrequency;   ///< Frequency ("width") of the first octave of noise (default to 1.0)
	P mAmplitude;   ///< Amplitude ("height") of the first octave of noise (default to 1.0)
	P mLacunarity;  ///< Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
	P mPersistence; ///< Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)

	inline uint8_t hash(int32_t i) const { return perm[reinterpret_cast<uint8_t&>(i)]; }

public:
	struct Output2
	{
		P r = 0, x = 0, y = 0;
	}; 
	
	struct Output3
	{
		P r = 0, x = 0, y = 0, z = 0;
	};

	// 1D Perlin simplex noise
	P Noise(P x) const;
	// 2D Perlin simplex noise
	P Noise(P x, P y) const;
	// 3D Perlin simplex noise
	Output3 Noise(P x, P y, P z) const;
	// faster check to see if there is any data you could process in between these coordinates
	P MaxNoise(P minX, P minY, P minZ, P maxX, P maxY, P maxZ) const;

	// Fractal/Fractional Brownian Motion (fBm) noise summation
	P Fractal(size_t octaves, P x) const;
	P Fractal(size_t octaves, P x, P y) const;
	Output3 Fractal(size_t octaves, P x, P y, P z) const;

	/**
	* Constructor of to initialize a fractal noise summation
	*
	* @param[in] frequency    Frequency ("width") of the first octave of noise (default to 1.0)
	* @param[in] amplitude    Amplitude ("height") of the first octave of noise (default to 1.0)
	* @param[in] lacunarity   Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
	* @param[in] persistence  Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
	*/
	explicit SimplexNoise(uint32_t seed, P frequency = P(1), P amplitude = P(1), P lacunarity = P(2), P persistence = P(0.5))
		: mFrequency(frequency)
		, mAmplitude(amplitude)
		, mLacunarity(lacunarity)
		, mPersistence(persistence)
	{
		std::mt19937 generator(seed);
		uint8_t* cur = perm;
		uint8_t* end = cur + 256;
		for (; cur < end; ++cur)
			*cur = (generator() % 256);
	}
};

#include "./SimplexNoise.inl"