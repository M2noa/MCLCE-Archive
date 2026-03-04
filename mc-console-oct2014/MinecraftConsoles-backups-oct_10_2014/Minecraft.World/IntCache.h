#pragma once

#include "ArrayWithLength.h"


class IntCache
{
private:

	class ThreadStorage
	{
	public:
		int maxSize;

		vector<intArray> tcache;
		vector<intArray> tallocated;
		
		vector<intArray> cache;
		vector<intArray> allocated;
		vector<intArray> toosmall;	// 4J added
		~ThreadStorage();
	};
	static DWORD tlsIdx;

	static const int TINY_CUTOFF = 256;

public:
	static intArray allocate(int size);
	static void releaseAll();

	static void CreateNewThreadStorage();
	static void ReleaseThreadStorage();
	static void Reset();		// 4J added
};