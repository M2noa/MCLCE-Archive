#pragma once
using namespace std;

class Hasher
{
private:
	wstring salt;

public:
	Hasher(wstring &salt);
	wstring getHash(wstring &name);
};