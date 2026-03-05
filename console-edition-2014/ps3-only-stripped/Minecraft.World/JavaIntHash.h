#pragma once

// Java doesn't have a default hash value for ints, however, the hashmap itself does some "supplemental" hashing, so
// our ints actually get hashed by code as implemented below. std templates *do* have a standard hash for ints, but it
// would appear to be a bit expensive so matching the java one for now anyway. This code implements the supplemental
// hashing that happens in java so we can match what their maps are doing with ints.

typedef struct
{
	int operator() (const int &k) const
	{
		int h = k;
        h += ~(h << 9);
        h ^=  (((unsigned int)h) >> 14);
        h +=  (h << 4);
        h ^=  (((unsigned int)h) >> 10);
		return h;
	}

} IntKeyHash;

typedef struct
{
	bool operator() (const int &x, const int &y) const { return x==y; }
} IntKeyEq;


// This hash functor is taken from the IntHashMap java class used by the game, so that we can use a standard std hashmap with this hash rather
// than implement the class itself
typedef struct
{
	int operator() (const int &k) const
	{
		unsigned int h = (unsigned int)k;
        h ^= (h >> 20) ^ (h >> 12);
        return (int)(h ^ (h >> 7) ^ (h >> 4));
	}
} IntKeyHash2;


// This hash functor is taken from the LongHashMap java class used by the game, so that we can use a standard std hashmap with this hash rather
// than implement the class itself
typedef struct
{
	int hash(const int &k) const
	{
		unsigned int h = (unsigned int)k;
        h ^= (h >> 20) ^ (h >> 12);
        return (int)(h ^ (h >> 7) ^ (h >> 4));
	}

	int operator() (const __int64 &k) const
	{
		return hash((int) ( k ^ (((__uint64)k) >> 32 )));
	}
} LongKeyHash;

typedef struct
{
	bool operator() (const __int64 &x, const __int64 &y) const { return x==y; }
} LongKeyEq;

typedef struct
{
	int operator() (const eINSTANCEOF &k) const
	{
		unsigned int h = (unsigned int)k;
        h ^= (h >> 20) ^ (h >> 12);
        return (int)(h ^ (h >> 7) ^ (h >> 4));
	}
} eINSTANCEOFKeyHash;

typedef struct
{
	bool operator() (const eINSTANCEOF &x, const eINSTANCEOF &y) const { return x==y; }
} eINSTANCEOFKeyEq;

