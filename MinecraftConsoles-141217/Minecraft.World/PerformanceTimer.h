#pragma once

class PerformanceTimer
{
private:
	LARGE_INTEGER  m_qwStartTime;
	float m_fSecsPerTick;

public:
	PerformanceTimer();
	void Reset();
	void PrintElapsedTime(const wstring &description);
};