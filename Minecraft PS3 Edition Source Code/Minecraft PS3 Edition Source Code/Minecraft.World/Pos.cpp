#include "stdafx.h"

#include "Pos.h"



Pos::Pos()
{
	x = y = z = 0;
}

Pos::Pos(int x, int y, int z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Pos::Pos(Pos *position)
{
	this->x = position->x;
	this->y = position->y;
	this->z = position->z;
}

//@Override
//public boolean equals(Object other)
bool Pos::equals(void *other)
{
	// TODO 4J Stu I cannot do a dynamic_cast from a void pointer
	// If I cast it to a Pos then do a dynamic_cast will it still return NULL if it wasn't originally a Pos?
	if (!( dynamic_cast<Pos *>( (Pos *)other ) != NULL ))
	{
		return false;
	}

	Pos *p = (Pos *) other;
	return x == p->x && y == p->y && z == p->z;
}

//@Override
int Pos::hashCode()
{
	return x + (z << 8) + (y << 16);
}

int Pos::compareTo(Pos *pos)
{
	if (y == pos->y)
	{
		if (z == pos->z)
		{
			return x - pos->x;
		}
		return z - pos->z;
	}
	return y - pos->y;
}

Pos *Pos::offset(int x, int y, int z)
{
	return new Pos(this->x + x, this->y + y, this->z + z);
}

void Pos::set(int x, int y, int z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Pos::set(Pos *pos)
{
	this->x = pos->x;
	this->y = pos->y;
	this->z = pos->z;
}

Pos *Pos::above()
{
	return new Pos(x, y + 1, z);
}

Pos *Pos::above(int steps)
{
	return new Pos(x, y + steps, z);
}

Pos *Pos::below()
{
	return new Pos(x, y - 1, z);
}

Pos *Pos::below(int steps)
{
	return new Pos(x, y - steps, z);
}

Pos *Pos::north()
{
	return new Pos(x, y, z - 1);
}

Pos *Pos::north(int steps)
{
	return new Pos(x, y, z - steps);
}

Pos *Pos::south()
{
	return new Pos(x, y, z + 1);
}

Pos *Pos::south(int steps)
{
	return new Pos(x, y, z + steps);
}

Pos *Pos::west()
{
	return new Pos(x - 1, y, z);
}

Pos *Pos::west(int steps)
{
	return new Pos(x - 1, y, z);
}

Pos *Pos::east()
{
	return new Pos(x + 1, y, z);
}

Pos *Pos::east(int steps)
{
	return new Pos(x + steps, y, z);
}

void Pos::move(int x, int y, int z)
{
	this->x += x;
	this->y += y;
	this->z += z;
}

void Pos::move(Pos pos)
{
	this->x += pos.x;
	this->y += pos.y;
	this->z += pos.z;
}

void Pos::moveX(int steps) 
{
	this->x += steps;
}

void Pos::moveY(int steps)
{
	this->y += steps;
}

void Pos::moveZ(int steps)
{
	this->z += steps;
}

void Pos::moveUp(int steps)
{
	this->y += steps;
}

void Pos::moveUp()
{
	this->y++;
}

void Pos::moveDown(int steps)
{
	this->y -= steps;
}

void Pos::moveDown()
{
	this->y--;
}

void Pos::moveEast(int steps)
{
	this->x += steps;
}

void Pos::moveEast()
{
	this->x++;
}

void Pos::moveWest(int steps)
{
	this->x -= steps;
}

void Pos::moveWest()
{
	this->x--;
}

void Pos::moveNorth(int steps)
{
	this->z -= steps;
}

void Pos::moveNorth()
{
	this->z--;
}

void Pos::moveSouth(int steps)
{
	this->z += steps;
}

void Pos::moveSouth()
{
	this->z++;
}

double Pos::dist(int x, int y, int z)
{
	int dx = this->x - x;
	int dy = this->y - y;
	int dz = this->z - z;

	return sqrt( (double) dx * dx + dy * dy + dz * dz);
}

double Pos::dist(Pos *pos)
{
	return dist(pos->x, pos->y, pos->z);
}

float Pos::distSqr(int x, int y, int z)
{
	int dx = this->x - x;
	int dy = this->y - y;
	int dz = this->z - z;
	return dx * dx + dy * dy + dz * dz;
}