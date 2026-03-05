#pragma once

class GameProgress
{
private:
	static GameProgress *instance;

public:
	static const long long UPDATE_FREQUENCY = 64 * 1000;

	static void Tick();
	static void Flush(int iPad);

protected:
	GameProgress();

	int			m_nextPad;
	long long	m_lastUpdate;

	void		updatePlayer(int iPad);

	float		calcGameProgress(int achievementsUnlocked);
};