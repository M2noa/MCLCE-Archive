#pragma once
class Mob;
class Options;
using namespace std;
#include "..\..\..\Minecraft.World\SoundTypes.h"


class SoundEngine : public ConsoleSoundEngine
{

public:
	SoundEngine();
	virtual void destroy();
	virtual void play(int iSound, float x, float y, float z, float volume, float pitch);
	virtual void playStreaming(const wstring& name, float x, float y , float z, float volume, float pitch, bool bMusicDelay=true);
	virtual void playUI(int iSound, float volume, float pitch);
	virtual void playMusicTick();
	virtual void updateMusicVolume(float fVal);
	virtual void updateSystemMusicPlaying(bool isPlaying);
	virtual void updateSoundEffectVolume(float fVal);
	virtual void init(Options *);
	virtual void tick(shared_ptr<Mob> *players, float a);	// 4J - updated to take array of local players rather than single one
	virtual void add(const wstring& name, File *file);
	virtual void addMusic(const wstring& name, File *file);
	virtual void addStreaming(const wstring& name, File *file);
#ifndef __PS3__
	static void setXACTEngine( IXACT3Engine *pXACT3Engine);
	void CreateStreamingWavebank(const char *pchName, IXACT3WaveBank **ppStreamedWaveBank);
	void CreateSoundbank(const char *pchName, IXACT3SoundBank **ppSoundBank);

#endif // __PS3__
	virtual char *ConvertSoundPathToName(const wstring& name, bool bConvertSpaces=false);
	bool isStreamingWavebankReady();		// 4J Added
		int initAudioHardware(int iMinSpeakers)	{ return iMinSpeakers;}

private:
#ifndef __PS3__
	static void XACTNotificationCallback( const XACT_NOTIFICATION* pNotification );
#endif // __PS3__
}; 