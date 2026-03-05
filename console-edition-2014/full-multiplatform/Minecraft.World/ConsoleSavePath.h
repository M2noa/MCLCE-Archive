#pragma once
using namespace std;

class ConsoleSavePath
{
private:
	wstring path;

public:
	ConsoleSavePath( const wstring &newPath ) { path = newPath; }

	wstring getName() const { return path; }

	wstring operator+( wstring &b ) { return path + b; }
};