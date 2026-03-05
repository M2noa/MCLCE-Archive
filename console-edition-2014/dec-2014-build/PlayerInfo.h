#pragma once
using namespace std;

class PlayerInfo
{
public:
	wstring name;
	int latency;

	PlayerInfo(const wstring &name)
	{
		this->name = name;
		latency = 0;
	}
};