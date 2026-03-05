#pragma once

class Pos {
public:

	Pos()
		: x(0)
		, y(0)
		, z(0){
	}

	Pos(int x, int y, int z)
		: x(x)
		, y(y)
		, z(z){
	}

	Pos(const Pos& position)
		: x(position.x)
		, y(position.y)
		, z(position.z) {
	}

	__inline bool operator==(const Pos& rhs) const {
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

	bool operator<(const Pos& rhs) const {
		return compareTo(rhs) < 0;
	}

	Pos operator-(const Pos& rhs) const {
		return Pos(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Pos operator+(int o) const {
		return Pos(x + o, y + o, z + o);
	}

	Pos operator+(const Pos& rhs) const {
		return Pos(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Pos operator *(int s) const {
		return Pos(x * s, y * s, z * s);
	}

	static int createHashCode(int x, int y, int z) {
		return x + (z << 8) + (y << 16);
	}

	int hashCode() const {
		return x + (z << 8) + (y << 16);
	}

	int compareTo(const Pos& pos) const {
		return hashCode() - pos.hashCode();
	}

	Pos offset(int _x, int _y, int _z) const {
		return Pos(x + _x, y + _y, z + _z);
	}

	void set(int _x, int _y, int _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	void set(const Pos& pos) {
		x = pos.x;
		y = pos.y;
		z = pos.z;
	}

	float distSqrToCenter(float _x, float _y, float _z) const {
		float dx = x + 0.5f - _x;
		float dy = y + 0.5f - _y;
		float dz = z + 0.5f - _z;
		return dx * dx + dy * dy + dz * dz;
	}

	int distSqr(int x2, int y2, int z2) const {
		int dx = x2 - x;
		int dy = y2 - y;
		int dz = z2 - z;
		return dx * dx + dy * dy + dz * dz;
	}

	int x, y, z;
};

namespace std {
	template<>
	struct hash<Pos>{

		size_t operator()(const Pos& x) const {
			return x.hashCode();
		}

	};

}
