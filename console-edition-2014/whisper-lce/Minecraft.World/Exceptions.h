#pragma once
using namespace std;

class EOFException : public std::exception
{

};

class IllegalArgumentException : public std::exception
{
public:
	wstring information;

	IllegalArgumentException(const wstring& information);
};

class IOException : public std::exception
{
public:
	wstring information;

	IOException(const wstring& information);
};

class RuntimeException : public std::exception
{
public:
	RuntimeException(const wstring& information);
};