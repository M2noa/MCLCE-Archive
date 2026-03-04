#pragma once

using namespace std;

class WstringLookup
{
private:
	UINT numIDs;
	unordered_map<wstring, UINT> str2int;
	vector<wstring> int2str;

public:
	WstringLookup();

	wstring lookup(UINT id);
	
	UINT lookup(wstring);

	VOID getTable(wstring **lookup, UINT *len);
};