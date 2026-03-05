#pragma once

class Rect2i
{
private:
	int xPos;
	int yPos;
	int width;
	int height;

public:
	Rect2i(int x, int y, int width, int height);

	Rect2i *intersect(const Rect2i *other);
	int getX() const;
	int getY() const;
	void setX(int x);
	void setY(int y);
	int getWidth() const;
	int getHeight() const;
	void setWidth(int width);
	void setHeight(int height);
	void setPosition(int x, int y);
};