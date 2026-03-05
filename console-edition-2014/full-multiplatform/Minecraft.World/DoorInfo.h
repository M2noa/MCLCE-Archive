#pragma once

class DoorInfo
{
public:
	const int x, y, z;
	const int insideDx, insideDz;
	int timeStamp;
	bool removed;

private:
	// this is used for mobs to see how many mobs are
	// present, it's not 100% accurate but may be good enough
	int bookings;

public:
	DoorInfo(int x, int y, int z, int insideDx, int insideDy, int timeStamp);

	int distanceTo(int x2, int y2, int z2);
	int distanceToSqr(int x2, int y2, int z2);
	int distanceToInsideSqr(int x2, int y2, int z2);
	int getIndoorX();
	int getIndoorY();
	int getIndoorZ();
	bool isInsideSide(int testX, int testZ);
	void resetBookingCount();
	void incBookingCount();
	int getBookingsCount();
};