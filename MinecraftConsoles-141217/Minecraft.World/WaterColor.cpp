#include "stdafx.h"

#include "WaterColor.h"

// 4J Stu - Not using this any more
//intArray WaterColor::pixels;
//
//void WaterColor::init(intArray pixels)
//{
//	int *oldData = WaterColor::pixels.data;
//	WaterColor::pixels = pixels;
//	delete [] oldData;
//}
//
//int WaterColor::get(double temp, double rain)
//{
//	rain *= temp;
//	int x = (int) ((1 - temp) * 255);
//	int y = (int) ((1 - rain) * 255);
//	int returnVal = pixels[y << 8 | x];
//
//	return returnVal;
//}