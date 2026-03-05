#include "stdafx.h"
#include "Rect2i.h"

Rect2i::Rect2i(int x, int y, int width, int height)
{
	xPos = x;
	yPos = y;
	this->width = width;
	this->height = height;
}

Rect2i *Rect2i::intersect(const Rect2i *other)
{
	int x0 = xPos;
	int y0 = yPos;
	int x1 = xPos + width;
	int y1 = yPos + height;

	int x2 = other->getX();
	int y2 = other->getY();
	int x3 = x2 + other->getWidth();
	int y3 = y2 + other->getHeight();

	xPos = max(x0, x2);
	yPos = max(y0, y2);
	width = max(0, min(x1, x3) - xPos);
	height = max(0, min(y1, y3) - yPos);

	return this;
}

int Rect2i::getX() const
{
	return xPos;
}

int Rect2i::getY() const
{
	return yPos;
}

void Rect2i::setX(int x)
{
	xPos = x;
}

void Rect2i::setY(int y)
{
	yPos = y;
}

int Rect2i::getWidth() const
{
	return width;
}

int Rect2i::getHeight() const
{
	return height;
}

void Rect2i::setWidth(int width)
{
	this->width = width;
}

void Rect2i::setHeight(int height)
{
	this->height = height;
}

void Rect2i::setPosition(int x, int y)
{
	xPos = x;
	yPos = y;
}