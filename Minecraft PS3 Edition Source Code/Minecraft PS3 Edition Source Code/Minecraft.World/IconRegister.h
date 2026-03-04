#pragma once
using namespace std;

class Icon;

class IconRegister
{
public:
	// 4J Stu - register is a reserved keyword in C++
	virtual Icon *registerIcon(const wstring &name) = 0;
	virtual int getIconType() = 0;
};