#include "stdafx.h"
#include "Consoles_SoundEngine.h"


bool ConsoleSoundEngine::GetIsPlayingStreamingCDMusic()				
{ 
	return m_bIsPlayingStreamingCDMusic;
}
bool ConsoleSoundEngine::GetIsPlayingStreamingGameMusic()			
{
	return m_bIsPlayingStreamingGameMusic;
}
void ConsoleSoundEngine::SetIsPlayingStreamingCDMusic(bool bVal)	
{
	m_bIsPlayingStreamingCDMusic=bVal;
}
void ConsoleSoundEngine::SetIsPlayingStreamingGameMusic(bool bVal)	
{
	m_bIsPlayingStreamingGameMusic=bVal;
}
bool ConsoleSoundEngine::GetIsPlayingEndMusic()						
{ 
	return m_bIsPlayingEndMusic;
}
bool ConsoleSoundEngine::GetIsPlayingNetherMusic()					
{ 
	return m_bIsPlayingNetherMusic;
}
void ConsoleSoundEngine::SetIsPlayingEndMusic(bool bVal)			
{
	m_bIsPlayingEndMusic=bVal;
}
void ConsoleSoundEngine::SetIsPlayingNetherMusic(bool bVal)			
{
	m_bIsPlayingNetherMusic=bVal;
}


