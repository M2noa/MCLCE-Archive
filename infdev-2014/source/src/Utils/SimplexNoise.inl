/**
* @file    SimplexNoise.cpp
* @brief   A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D).
*
* Copyright (c) 2014-2018 Sebastien Rombauts (sebastien.rombauts@gmail.com)
*
* This C++ implementation is based on the speed-improved Java version 2012-03-09
* by Stefan Gustavson (original Java source code in the public domain).
* http://webstaff.itn.liu.se/~stegu/simplexnoise/SimplexNoise.java:
* - Based on example code by Stefan Gustavson (stegu@itn.liu.se).
* - Optimisations by Peter Eastman (peastman@drizzle.stanford.edu).
* - Better rank ordering method by Stefan Gustavson in 2012.
*
* This implementation is "Simplex Noise" as presented by
* Ken Perlin at a relatively obscure and not often cited course
* session "Real-Time Shading" at Siggraph 2001 (before real
* time shading actually took on), under the title "hardware noise".
* The 3D function is numerically equivalent to his Java reference
* code available in the PDF course notes, although I re-implemented
* it from scratch to get more readable code. The 1D, 2D and 4D cases
* were implemented from scratch by me from Ken Perlin's text.
*
* Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
* or copy at http://opensource.org/licenses/MIT)
*/

#include "SimplexNoise.h"

#include <cstdint>  // int32_t/uint8_t
#include <cmath>
#include <algorithm>

/**
* Computes the largest integer value not greater than the P one
*
* This method is faster than using (int32_t)std::floor(fp).
*
* I measured it to be approximately twice as fast:
*  P:  ~18.4ns instead of ~39.6ns on an AMD APU),
*  double: ~20.6ns instead of ~36.6ns on an AMD APU),
* Reference: http://www.codeproject.com/Tips/700780/Fast-floor-ceiling-functions
*
* @param[in] fp    P input value
*
* @return largest integer value not greater than fp
*/
template<class P>
static inline int32_t fastfloor(P fp)
{
	int32_t i = static_cast<int32_t>(fp);
	return (fp < i) ? (i - 1) : (i);
}

/**
* Permutation table. This is just a random jumble of all numbers 0-255.
*
* This produce a repeatable pattern of 256, but Ken Perlin stated
* that it is not a problem for graphic texture as the noise features disappear
* at a distance far enough to be able to see a repeatable pattern of 256.
*
* This needs to be exactly the same for all instances on all platforms,
* so it's easiest to just keep it as static explicit data.
* This also removes the need for any initialisation of this class.
*
* Note that making this an uint32_t[] instead of a uint8_t[] might make the
* code run faster on platforms with a high penalty for unaligned single
* byte addressing. Iel x86 is generally single-byte-friendly, but
* some other CPUs are faster with 4-aligned reads.
* However, a char[] is smaller, which avoids cache trashing, and that
* is probably the most important aspect on most architectures.
* This array is accessed a *lot* by the noise functions.
* A vector-valued noise over 3D accesses it 96 times, and a
* P-valued 4D noise 64 times. We want this to fit in the cache!
*/
/*static const uint8_t perm[256] =
{
	151, 160, 137, 91, 90, 15,
	131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
	190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
	88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
	77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
	102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
	135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
	5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
	223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
	129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
	251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
	49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
	138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};*/

/**
* Helper function to hash an integer using the above permutation table
*
*  This inline function costs around 1ns, and is called N+1 times for a noise of N dimension.
*
*  Using a real hash function would be better to improve the "repeatability of 256" of the above permutation table,
* but fast integer Hash functions uses more time and have bad random properties.
*
* @param[in] i integer value to hash
*
* @return 8-bits hashed value
*/


/* NOTE Gradient table to test if lookup-table are more efficient than calculs
template<class P>
static const P gradients1D[16] =
{
	P(-8), P(-7), P(-6), P(-5), P(-4), P(-3), P(-2), P(-1),
	P(1),  P(2),  P(3),  P(4),  P(5),  P(6),  P(7),  P(8)
};
*/

/**
* Helper function to compute gradients-dot-residual vectors (1D)
*
* @note that these generate gradients of more than unit length. To make
* a close match with the value range of classic Perlin noise, the final
* noise values need to be rescaled to fit nicely within [-1,1].
* (The simplex noise functions as such also have different scaling.)
* Note also that these noise functions are the most practical and useful
* signed version of Perlin noise.
*
* @param[in] hash  hash value
* @param[in] x     distance to the corner
*
* @return gradient value
*/
template<class P>
static P grad(int32_t hash, P x)
{
	const int32_t h = hash & 0x0F;  // Convert low 4 bits of hash code
	P grad = P(1) + (h & 7);    // Gradient value 1.0, 2.0, ..., 8.0
	if ((h & 8) != 0)
		grad = -grad; // Set a random sign for the gradient
									//  P grad = gradients1D[h];    // NOTE : Test of Gradient look-up table instead of the above
	return grad * x;              // Multiply the gradient with the distance
}

/**
* Helper functions to compute gradients-dot-residual vectors (2D)
*
* @param[in] hash  hash value
* @param[in] x     x coord of the distance to the corner
* @param[in] y     y coord of the distance to the corner
*
* @return gradient value
*/
template<class P, class I = int>
static P grad(int32_t hash, P x, P y)
{
	const int32_t h = hash & 0x3F;  // Convert low 3 bits of hash code
	const P u = h < 4 ? x : y;  // Io 8 simple gradient directions,
	const P v = h < 4 ? y : x;
	return ((h & 1) ? -u : u) + ((h & 2) ? -P(2) * v : P(2) * v); // and compute the dot product with (x,y).
}

/**
* Helper functions to compute gradients-dot-residual vectors (3D)
*
* @param[in] hash  hash value
* @param[in] x     x coord of the distance to the corner
* @param[in] y     y coord of the distance to the corner
* @param[in] z     z coord of the distance to the corner
*
* @return gradient value
*/
template<class P, class I = int>
static P grad(int32_t hash, P x, P y, P z)
{
	I h = hash & 15;     // Convert low 4 bits of hash code into 12 simple
	P u = h < 8 ? x : y; // gradient directions, and compute dot product.
	P v = h < 4 ? y : h == 12 || h == 14 ? x : z; // Fix repeats at h = 12 to 15
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

/**
* 1D Perlin simplex noise
*
*  Takes around 74ns on an AMD APU.
*
* @param[in] x P coordinate
*
* @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
*/
template<class P, class I>
P SimplexNoise<P, I>::Noise(P x) const
{
	P n0, n1;   // Noise contributions from the two "corners"

	// No need to skew the input space in 1D

	// Corners coordinates (nearest integer values):
	int32_t i0 = fastfloor(x);
	int32_t i1 = i0 + 1;
	// Distances to corners (between 0 and 1):
	P x0 = x - i0;
	P x1 = x0 - P(1);

	// Calculate the contribution from the first corner
	P t0 = P(1) - x0 * x0;
	//  if(t0 < 0.0f) t0 = 0.0f; // not possible
	t0 *= t0;
	n0 = t0 * t0 * grad(hash(i0), x0);

	// Calculate the contribution from the second corner
	P t1 = P(1) - x1 * x1;
	//  if(t1 < 0.0f) t1 = 0.0f; // not possible
	t1 *= t1;
	n1 = t1 * t1 * grad(hash(i1), x1);

	// The maximum value of this noise is 8*(3/4)^4 = 2.53125
	// A factor of 0.395 scales to fit exactly within [-1,1]
	return P(0.395) * (n0 + n1);
}

/**
* 2D Perlin simplex noise
*
*  Takes around 150ns on an AMD APU.
*
* @param[in] x P coordinate
* @param[in] y P coordinate
*
* @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
*/
template<class P, class I>
P SimplexNoise<P, I>::Noise(P x, P y) const
{
	P n0, n1, n2;   // Noise contributions from the three corners

	// Skewing/Unskewing factors for 2D
	static const P F2 = P(0.366025403);  // F2 = (sqrt(3) - 1) / 2
	static const P G2 = P(0.211324865);  // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

	// Skew the input space to determine which simplex cell we're in
	const P s = (x + y) * F2;  // Hairy factor for 2D
	const P xs = x + s;
	const P ys = y + s;
	const int32_t i = fastfloor(xs);
	const int32_t j = fastfloor(ys);

	// Unskew the cell origin back to (x,y) space
	const P t = static_cast<P>(i + j) * G2;
	const P X0 = i - t;
	const P Y0 = j - t;
	const P x0 = x - X0;  // The x,y distances from the cell origin
	const P y0 = y - Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int32_t i1, j1;  // Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0 > y0) {   // lower triangle, XY order: (0,0)->(1,0)->(1,1)
		i1 = 1;
		j1 = 0;
	}
	else {   // upper triangle, YX order: (0,0)->(0,1)->(1,1)
		i1 = 0;
		j1 = 1;
	}

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6

	const P x1 = x0 - i1 + G2;            // Offsets for middle corner in (x,y) unskewed coords
	const P y1 = y0 - j1 + G2;
	const P x2 = x0 - P(1) + P(2) * G2;   // Offsets for last corner in (x,y) unskewed coords
	const P y2 = y0 - P(1) + P(2) * G2;

	// Work out the hashed gradient indices of the three simplex corners
	const I gi0 = hash(i + hash(j));
	const I gi1 = hash(i + i1 + hash(j + j1));
	const I gi2 = hash(i + 1 + hash(j + 1));

	// Calculate the contribution from the first corner
	P t0 = P(0.5) - x0 * x0 - y0 * y0;
	if (t0 < P(0)) {
		n0 = P(0);
	}
	else {
		t0 *= t0;
		n0 = t0 * t0 * grad(gi0, x0, y0);
	}

	// Calculate the contribution from the second corner
	P t1 = P(0.5) - x1 * x1 - y1 * y1;
	if (t1 < P(0)) {
		n1 = P(0);
	}
	else {
		t1 *= t1;
		n1 = t1 * t1 * grad(gi1, x1, y1);
	}

	// Calculate the contribution from the third corner
	P t2 = P(0.5) - x2 * x2 - y2 * y2;
	if (t2 < P(0)) {
		n2 = P(0);
	}
	else {
		t2 *= t2;
		n2 = t2 * t2 * grad(gi2, x2, y2);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the Ierval [-1,1].
	return P(45.23065) * (n0 + n1 + n2);
}

#include <chrono>

/**
* 3D Perlin simplex noise
*
* @param[in] x P coordinate
* @param[in] y P coordinate
* @param[in] z P coordinate
*
* @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
*/
template<class P, class I>
typename SimplexNoise<P, I>::Output3 SimplexNoise<P, I>::Noise(P x, P y, P z) const
{
	// Skewing/Unskewing factors for 3D
	static const P F3 = P(1) / P(3);
	static const P G3 = P(1) / P(6);

	// Skew the input space to determine which simplex cell we're in
	P s = (x + y + z) * F3; // Very nice and simple skew factor for 3D
	I a = fastfloor(x + s);
	I b = fastfloor(y + s);
	I c = fastfloor(z + s);

	// The x,y,z distances from the cell origin
	P t = (a + b + c) * G3;
	P x0 = x - (a - t); 
	P y0 = y - (b - t);
	P z0 = z - (c - t);

	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
	I a1, b1, c1; // Offsets for second corner of simplex in (a,b,c) coords
	I a2, b2, c2; // Offsets for third corner of simplex in (a,b,c) coords

	// Determine which simplex we are in.
	if (x0 >= y0)
	{
		b2 = y0 >= z0;
		a1 = b2 || x0 >= z0;
		b1 = 0;
		a2 = 1;
		c1 = 1 - a1;
		c2 = 1 - b2;
	}
	else // x0 < y0
	{
		b1 = y0 >= z0;
		c1 = 1 - b1;
		c2 = c1 || x0 < z0;
		a2 = 1 - c2;
		a1 = 0;
		b2 = 1;
	}

	// A step of (1,0,0) in (a,b,c) means a step of (1-d,-d,-d) in (x,y,z),
	// a step of (0,1,0) in (a,b,c) means a step of (-d,1-d,-d) in (x,y,z), and
	// a step of (0,0,1) in (a,b,c) means a step of (-d,-d,1-d) in (x,y,z), where
	// d = 1/6.
	P x1 = x0 - a1 + G3; // Offsets for second corner in (x,y,z) coords
	P y1 = y0 - b1 + G3;
	P z1 = z0 - c1 + G3;
	P x2 = x0 - a2 + P(2) * G3; // Offsets for third corner in (x,y,z) coords
	P y2 = y0 - b2 + P(2) * G3;
	P z2 = z0 - c2 + P(2) * G3;
	P x3 = x0 - P(1) + P(3) * G3; // Offsets for last corner in (x,y,z) coords
	P y3 = y0 - P(1) + P(3) * G3;
	P z3 = z0 - P(1) + P(3) * G3;
	
	// Work out the hashed gradient indices of the four simplex corners
	I gi0 = hash(a + hash(b + hash(c)));
	I gi1 = hash(a + a1 + hash(b + b1 + hash(c + c1)));
	I gi2 = hash(a + a2 + hash(b + b2 + hash(c + c2)));
	I gi3 = hash(a + 1 + hash(b + 1 + hash(c + 1)));
	
	// Noise contributions from the four corners
	P n0, n1, n2, n3;

	// Calculate the contribution from the four corners
	P t0 = P(0.6) - x0 * x0 - y0 * y0 - z0 * z0;
	n0 = t0 < P(0) ? P(0) : (t0 *= t0) * t0 * grad(gi0, x0, y0, z0);

	P t1 = P(0.6) - x1 * x1 - y1 * y1 - z1 * z1;
	n1 = t1 < P(0) ? P(0) : (t1 *= t1) * t1 * grad(gi1, x1, y1, z1);

	P t2 = P(0.6) - x2 * x2 - y2 * y2 - z2 * z2;
	n2 = t2 < P(0) ? P(0) : (t2 *= t2) * t2 * grad(gi2, x2, y2, z2);

	P t3 = P(0.6) - x3 * x3 - y3 * y3 - z3 * z3;
	n3 = t3 < P(0) ? P(0) : (t3 *= t3) * t3 * grad(gi3, x3, y3, z3);
	
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to stay just inside [-1,1]

	return { P(32) * (n0 + n1 + n2 + n3), n0, n1, n2 };
}

template<class P, class I>
P SimplexNoise<P, I>::MaxNoise(P minX, P minY, P minZ, P maxX, P maxY, P maxZ) const
{
	static const P F3 = P(1) / P(3);
	static const P G3 = P(1) / P(6);

	P maxValue = -std::numeric_limits<P>::infinity();
	for (P x = minX; x <= maxX; ++x)
	{
		for (P y = minY; y <= maxY; ++y)
		{
			for (P z = minZ; z <= maxZ; ++z)
			{
				// Very nice and simple skew factor for 3D
				P s = (x + y + z) * F3;
				I a = fastfloor(x + s);
				I b = fastfloor(y + s);
				I c = fastfloor(z + s);

				// The x,y,z distances from the cell origin
				P t = (a + b + c) * G3;
				P x0 = x - (a - t);
				P y0 = y - (b - t);
				P z0 = z - (c - t);
				
				P t0 = P(0.6) - x0 * x0 - y0 * y0 - z0 * z0;
				P n0 = t0 < P(0) ? P(0) : (t0 *= t0) * t0 * grad(hash(a + hash(b + hash(c))), x0, y0, z0);

				maxValue = std::fmaxf(n0, maxValue);
			}
		}
	}

	return maxValue;
}


/**
* Fractal/Fractional Brownian Motion (fBm) summation of 1D Perlin Simplex noise
*
* @param[in] octaves   number of fraction of noise to sum
* @param[in] x         P coordinate
*
* @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
*/
template<class P, class I>
P SimplexNoise<P, I>::Fractal(size_t octaves, P x) const
{
	P output = P(0);
	P denom = P(0);
	P frequency = mFrequency;
	P amplitude = mAmplitude;

	for (size_t i = 0; i < octaves; ++i)
	{
		output += amplitude * noise(x * frequency);
		denom += amplitude;

		frequency *= mLacunarity;
		amplitude *= mPersistence;
	}

	return output / denom;
}

/**
* Fractal/Fractional Brownian Motion (fBm) summation of 2D Perlin Simplex noise
*
* @param[in] octaves   number of fraction of noise to sum
* @param[in] x         x P coordinate
* @param[in] y         y P coordinate
*
* @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
*/
template<class P, class I>
P SimplexNoise<P, I>::Fractal(size_t octaves, P x, P y) const
{
	P output = P(0);
	P denom = P(0);
	P frequency = mFrequency;
	P amplitude = mAmplitude;

	for (size_t i = 0; i < octaves; ++i)
	{
		output += amplitude * Noise(x * frequency, y * frequency);
		denom += amplitude;

		frequency *= mLacunarity;
		amplitude *= mPersistence;
	}

	return output / denom;
}

/**
* Fractal/Fractional Brownian Motion (fBm) summation of 3D Perlin Simplex noise
*
* @param[in] octaves   number of fraction of noise to sum
* @param[in] x         x P coordinate
* @param[in] y         y P coordinate
* @param[in] z         z P coordinate
*
* @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
*/
template<class P, class I>
typename SimplexNoise<P, I>::Output3 SimplexNoise<P, I>::Fractal(size_t octaves, P x, P y, P z) const
{
	P denom = P(0);
	P frequency = mFrequency;
	P amplitude = mAmplitude;

	Output3 output;

	for (size_t i = 0; i < octaves; ++i)
	{
		Output3 o = Noise(x * frequency, y * frequency, z * frequency);

		output.r += amplitude * o.r;
		output.x += o.x;
		output.y += o.y;
		output.z += o.z;

		denom += amplitude;

		frequency *= mLacunarity;
		amplitude *= mPersistence;
	}

	const P normalMultiplier = P(1) / P(octaves);
	return { output.r / denom, output.x * normalMultiplier, output.y * normalMultiplier, output.z * normalMultiplier };
}