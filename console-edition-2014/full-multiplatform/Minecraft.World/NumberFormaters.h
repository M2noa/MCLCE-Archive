#pragma once
using namespace std;

// 4J Stu - The java formated numbers based on a local passed in, but I am just going for a constant format here
class NumberFormat
{
public:
	static wstring format(int value)
	{
		// TODO 4J Stu - Change the length of the formatted number
		wchar_t output[256];
		swprintf( output, 256, L"%d", value);
		wstring result = wstring( output );
		return result;
	}
};


class DecimalFormat
{
private:
	const wstring formatString;
public:
	wstring format(double value)
	{
		// TODO 4J Stu - Change the length of the formatted number
		wchar_t output[256];
		swprintf( output, 256, formatString.c_str(), value);
		wstring result = wstring( output );
		return result;
	}

	// 4J Stu - The java code took a string format, we take a printf format string
	DecimalFormat(wstring x) : formatString( x ) {};
};