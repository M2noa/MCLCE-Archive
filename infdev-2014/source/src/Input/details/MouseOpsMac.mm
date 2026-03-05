#pragma once


inline void GetMouseMoveDelta(int32_t* x, int32_t* y)
{
	CGGetLastMouseDelta(x, y);
}