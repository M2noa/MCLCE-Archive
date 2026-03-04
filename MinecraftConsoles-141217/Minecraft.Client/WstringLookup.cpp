
#include "stdafx.h"

#include "WstringLookup.h"

WstringLookup::WstringLookup()
{
	numIDs = 0;
}

wstring WstringLookup::lookup(UINT id)
{
	// TODO
	//if (id > currentMaxID)
	//	throw error

	return int2str.at(id);
}
	
UINT WstringLookup::lookup(wstring str)
{
	if (str2int.find(str) == str2int.end())
	{
		pair<wstring,UINT> p = 
			pair<wstring,UINT>(str, numIDs);

		str2int.insert( p );
		int2str.push_back( str );

		return numIDs++;
	}
	else
	{
		return str2int.at(str);
	}
}

VOID WstringLookup::getTable(wstring **lookup, UINT *len)
{
	// Outputs
	wstring *out_lookup; UINT out_len;

	// Fill lookup.
	out_lookup = new wstring[int2str.size()];
	for (UINT i = 0; i < numIDs; i++)
		out_lookup[i] = int2str.at(i);

	out_len = numIDs;

	// Return.
	*lookup = out_lookup;
	*len = out_len;
	return;
}