#include "stdafx.h"
#include "GrassColor.h"

// 4J Stu - Don't want to do this any more
//intArray GrassColor::pixels;
//
//void GrassColor::init(intArray pixels)
//{
//	int *oldData = GrassColor::pixels.data;
//	GrassColor::pixels = pixels;
//	delete [] oldData;
//}
//
//int GrassColor::get(double temp, double rain)
//{
//    rain *= temp;
//    int x = (int) ((1 - temp) * 255);
//    int y = (int) ((1 - rain) * 255);
//	int returnVal = pixels[y << 8 | x];	
//	return returnVal;
//}