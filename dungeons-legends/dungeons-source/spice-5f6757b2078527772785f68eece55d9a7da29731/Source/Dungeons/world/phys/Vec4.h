#pragma once

class Vec4 {
public:

	Vec4() :
		Vec4(0.0f, 0.0f, 0.0f, 0.0f){
	}

	Vec4(float _x, float _y, float _z, float _w) :
		x(_x)
		, y(_y)
		, z(_z)
		, w(_w) {
	}

	Vec4* set(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
		return this;
	}

	std::string toString() const {
		std::stringstream ss;
		ss << "Vec4(" << x << "," << y << "," << z << "," << w << ")";
		return ss.str();
	}

	bool isNan() const {
		return Math::isNan(x) ||
			   Math::isNan(y) ||
			   Math::isNan(z) ||
			   Math::isNan(w);
	}

	float x, y, z, w;
};
