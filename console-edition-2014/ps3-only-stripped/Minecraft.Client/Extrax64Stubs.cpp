#include "stdafx.h"
#ifndef __PS3__
//#include <compressapi.h>
#endif // __PS3__

#if defined(__PS3__)
#include "PS3\Sentient\SentientManager.h"
#include "StatsCounter.h"
#include "PS3\Social\SocialManager.h"
#include <libsn.h>
#include <libsntuner.h>
#else
#include "Orbis\Sentient\SentientManager.h"
#include "StatsCounter.h"
#include "Orbis\Social\SocialManager.h"
#include "Orbis\Sentient\DynamicConfigurations.h"
#include <perf.h>
#endif

#if !defined(__PS3__)
#endif
CSentientManager SentientManager;
CXuiStringTable StringTable;

ATG::XMLParser::XMLParser() {}
ATG::XMLParser::~XMLParser() {}
HRESULT    ATG::XMLParser::ParseXMLBuffer( CONST CHAR* strBuffer, UINT uBufferSize ) { return S_OK; }   
VOID ATG::XMLParser::RegisterSAXCallbackInterface( ISAXCallback *pISAXCallback ) {}

bool	CSocialManager::IsTitleAllowedToPostAnything() { return false; }
bool	CSocialManager::AreAllUsersAllowedToPostImages() { return false; }
bool	CSocialManager::IsTitleAllowedToPostImages() { return false; }

bool	CSocialManager::PostLinkToSocialNetwork( ESocialNetwork eSocialNetwork, DWORD dwUserIndex, bool bUsingKinect ) { return false; }
bool	CSocialManager::PostImageToSocialNetwork( ESocialNetwork eSocialNetwork, DWORD dwUserIndex, bool bUsingKinect ) { return false; }
CSocialManager *CSocialManager::Instance() { return NULL; }
void CSocialManager::SetSocialPostText(LPCWSTR Title, LPCWSTR Caption, LPCWSTR Desc) {};

DWORD XShowPartyUI(DWORD dwUserIndex) { return 0; }
DWORD XShowFriendsUI(DWORD dwUserIndex) { return 0; }
HRESULT XPartyGetUserList(XPARTY_USER_LIST *pUserList) { return S_OK; }
DWORD XContentGetThumbnail(DWORD dwUserIndex, const XCONTENT_DATA *pContentData,  PBYTE pbThumbnail,  PDWORD pcbThumbnail,  PXOVERLAPPED *pOverlapped) { return 0; }
void XShowAchievementsUI(int i) {}
DWORD XBackgroundDownloadSetMode(XBACKGROUND_DOWNLOAD_MODE Mode) { return 0; }

void PIXAddNamedCounter(int a, char *b, ...) {}
//#define PS3_USE_PIX_EVENTS 
//#define PS4_USE_PIX_EVENTS 
void PIXBeginNamedEvent(int a, char *b, ...)
{
#ifdef PS4_USE_PIX_EVENTS
	char buf[512];
    va_list args;
    va_start(args,b);
    vsprintf(buf,b,args);
	sceRazorCpuPushMarker(buf, 0xffffffff, SCE_RAZOR_MARKER_ENABLE_HUD);

#endif
#ifdef PS3_USE_PIX_EVENTS
	char buf[256];
	wchar_t wbuf[256];
    va_list args;
    va_start(args,b);
    vsprintf(buf,b,args);
	snPushMarker(buf);

// 	mbstowcs(wbuf,buf,256);
// 	RenderManager.BeginEvent(wbuf);
    va_end(args);
#endif
}
#if 0//0
	if( PixDepth < 64 )
	{
		char buf[512];
		va_list args;
		va_start(args,b);
		vsprintf(buf,b,args);
		sceRazorCpuPushMarkerWithHud(buf, 0xffffffff, SCE_RAZOR_MARKER_ENABLE_HUD);
	}
	PixDepth += 1;
#endif


void PIXEndNamedEvent()
{
#ifdef PS4_USE_PIX_EVENTS
	sceRazorCpuPopMarker();
#endif
#ifdef PS3_USE_PIX_EVENTS
	snPopMarker();
// 	RenderManager.EndEvent();
#endif
#if 0//0
	if( PixDepth <= 64 )
	{
		sceRazorCpuPopMarker();
	}
	PixDepth -= 1;
#endif
}
void PIXSetMarkerDeprecated(int a, char *b, ...) {}

// void *D3DXBUFFER::GetBufferPointer() { return NULL; }
// int D3DXBUFFER::GetBufferSize() { return 0; }
// void D3DXBUFFER::Release() {}

// #ifdef _DURANGO
// void GetLocalTime(SYSTEMTIME *time) {}
// #endif


bool IsEqualXUID(PlayerUID a, PlayerUID b)
{
#if defined(__PS3__)
	return (a == b);
#else
	return false;
#endif
}

void XMemCpy(void *a, const void *b, size_t s) { memcpy(a, b, s); }
void XMemSet(void *a, int t, size_t s) { memset(a, t, s); }
void XMemSet128(void *a, int t, size_t s) { memset(a, t, s); }
void *XPhysicalAlloc(SIZE_T a, ULONG_PTR  b, ULONG_PTR c, DWORD d) { return malloc(a); }
void XPhysicalFree(void *a) { free(a); }

D3DXVECTOR3::D3DXVECTOR3() {}
D3DXVECTOR3::D3DXVECTOR3(float x,float y,float z) : x(x), y(y), z(z) {}
D3DXVECTOR3& D3DXVECTOR3::operator += ( CONST D3DXVECTOR3& add ) { x += add.x; y += add.y; z += add.z; return *this; }

BYTE IQNetPlayer::GetSmallId() { return 0; }
void IQNetPlayer::SendData(IQNetPlayer *player, const void *pvData, DWORD dwDataSize, DWORD dwFlags)
{
	app.DebugPrintf("Sending from 0x%x to 0x%x %d bytes\n",this,player,dwDataSize);
}
bool IQNetPlayer::IsSameSystem(IQNetPlayer *player) { return true; }
DWORD IQNetPlayer::GetSendQueueSize( IQNetPlayer *player, DWORD dwFlags ) { return 0; }
DWORD IQNetPlayer::GetCurrentRtt() { return 0; }
bool IQNetPlayer::IsHost() { return this == &IQNet::m_player[0]; }
bool IQNetPlayer::IsGuest() { return false; }
bool IQNetPlayer::IsLocal() { return true; }
PlayerUID IQNetPlayer::GetXuid() { return INVALID_XUID; }
LPCWSTR IQNetPlayer::GetGamertag() { static const wchar_t *test = L"stub"; return test; }
int IQNetPlayer::GetSessionIndex() { return 0; }
bool IQNetPlayer::IsTalking() { return false; }
bool IQNetPlayer::IsMutedByLocalUser(DWORD dwUserIndex) { return false; }
bool IQNetPlayer::HasVoice() { return false; }
bool IQNetPlayer::HasCamera() { return false; }
int IQNetPlayer::GetUserIndex() { return this - &IQNet::m_player[0]; }
void IQNetPlayer::SetCustomDataValue(ULONG_PTR ulpCustomDataValue) {
	m_customData = ulpCustomDataValue;
}
ULONG_PTR IQNetPlayer::GetCustomDataValue() {
	return m_customData;
}

IQNetPlayer IQNet::m_player[4];

bool _bQNetStubGameRunning = false;

HRESULT IQNet::AddLocalPlayerByUserIndex(DWORD dwUserIndex){ return S_OK; }
IQNetPlayer *IQNet::GetHostPlayer() { return &m_player[0]; }
IQNetPlayer *IQNet::GetLocalPlayerByUserIndex(DWORD dwUserIndex) { return &m_player[dwUserIndex]; } 
IQNetPlayer *IQNet::GetPlayerByIndex(DWORD dwPlayerIndex) { return &m_player[0]; }
IQNetPlayer *IQNet::GetPlayerBySmallId(BYTE SmallId){ return &m_player[0]; }
IQNetPlayer *IQNet::GetPlayerByXuid(PlayerUID xuid){ return &m_player[0]; }
DWORD IQNet::GetPlayerCount() { return 1; }
QNET_STATE IQNet::GetState() { return _bQNetStubGameRunning ? QNET_STATE_GAME_PLAY : QNET_STATE_IDLE; }
bool IQNet::IsHost() { return true; }
HRESULT IQNet::JoinGameFromInviteInfo(DWORD dwUserIndex, DWORD dwUserMask, const INVITE_INFO *pInviteInfo) { return S_OK; }
void IQNet::HostGame() { _bQNetStubGameRunning = true; }
void IQNet::EndGame() { _bQNetStubGameRunning = false; }

DWORD MinecraftDynamicConfigurations::GetTrialTime() { return DYNAMIC_CONFIG_DEFAULT_TRIAL_TIME; }

void XSetThreadProcessor(HANDLE a, int b) {}
// #if !(defined __PS3__) && !(defined __ORBIS__)
// BOOL XCloseHandle(HANDLE a) { return CloseHandle(a); }
// #endif // __PS3__

DWORD XUserGetSigninInfo(
         DWORD dwUserIndex,
         DWORD dwFlags,
         PXUSER_SIGNIN_INFO pSigninInfo
)
{
	return 0;
}

LPCWSTR CXuiStringTable::Lookup(LPCWSTR szId) { return szId; }
LPCWSTR CXuiStringTable::Lookup(UINT nIndex) { return L"String"; }
void CXuiStringTable::Clear() {}
HRESULT CXuiStringTable::Load(LPCWSTR szId) { return S_OK; }

DWORD XUserAreUsersFriends( DWORD dwUserIndex, PPlayerUID pXuids, DWORD dwXuidCount, PBOOL pfResult, void *pOverlapped) { return 0; }

#if defined __PS3__
#else
HRESULT XMemDecompress(
         XMEMDECOMPRESSION_CONTEXT Context,
         VOID *pDestination,
         SIZE_T *pDestSize,
         CONST VOID *pSource,
         SIZE_T SrcSize
)
{
	memcpy(pDestination, pSource, SrcSize);
	*pDestSize = SrcSize;
	return S_OK;

	/*
	DECOMPRESSOR_HANDLE Decompressor    = (DECOMPRESSOR_HANDLE)Context;
	if( Decompress(
        Decompressor,           //  Decompressor handle
        (void *)pSource,		//  Compressed data
        SrcSize,				//  Compressed data size
        pDestination,			//  Decompressed buffer
        *pDestSize,				//  Decompressed buffer size
        pDestSize) )				//  Decompressed data size
	{
		return S_OK;
	}
	else
	*/
	{
		return E_FAIL;
	}
}

HRESULT XMemCompress(
         XMEMCOMPRESSION_CONTEXT Context,
         VOID *pDestination,
         SIZE_T *pDestSize,
         CONST VOID *pSource,
         SIZE_T SrcSize
)
{
	memcpy(pDestination, pSource, SrcSize);
	*pDestSize = SrcSize;
	return S_OK;

	/*
	COMPRESSOR_HANDLE Compressor    = (COMPRESSOR_HANDLE)Context;
	if( Compress(
			Compressor,                  //  Compressor Handle
			(void *)pSource,             //  Input buffer, Uncompressed data
			SrcSize,					 //  Uncompressed data size
			pDestination,                //  Compressed Buffer
			*pDestSize,                  //  Compressed Buffer size
			pDestSize)	)				//  Compressed Data size
	{
		return S_OK;
	}
	else
	*/
	{
		return E_FAIL;
	}
}

HRESULT XMemCreateCompressionContext(
         XMEMCODEC_TYPE CodecType,
         CONST VOID *pCodecParams,
         DWORD Flags,
         XMEMCOMPRESSION_CONTEXT *pContext
)
{
	/*
	COMPRESSOR_HANDLE Compressor    = NULL;

	HRESULT hr = CreateCompressor(
		COMPRESS_ALGORITHM_XPRESS_HUFF, //  Compression Algorithm
		NULL,                           //  Optional allocation routine
		&Compressor);                   //  Handle

	pContext = (XMEMDECOMPRESSION_CONTEXT *)Compressor;
	return hr;
	*/
	return 0;
}

HRESULT XMemCreateDecompressionContext(
         XMEMCODEC_TYPE CodecType,
         CONST VOID *pCodecParams,
         DWORD Flags,
         XMEMDECOMPRESSION_CONTEXT *pContext
)
{
	/*
	DECOMPRESSOR_HANDLE  Decompressor    = NULL;

	HRESULT hr = CreateDecompressor(
		COMPRESS_ALGORITHM_XPRESS_HUFF, //  Compression Algorithm
		NULL,                           //  Optional allocation routine
		&Decompressor);                   //  Handle

	pContext = (XMEMDECOMPRESSION_CONTEXT *)Decompressor;
	return hr;
	*/
	return 0;
}

void XMemDestroyCompressionContext(XMEMCOMPRESSION_CONTEXT Context)
{
//	COMPRESSOR_HANDLE Compressor    = (COMPRESSOR_HANDLE)Context;
//	CloseCompressor(Compressor);
}

void XMemDestroyDecompressionContext(XMEMDECOMPRESSION_CONTEXT Context)
{
//	DECOMPRESSOR_HANDLE Decompressor    = (DECOMPRESSOR_HANDLE)Context;
//	CloseDecompressor(Decompressor);
}
#endif

//#ifndef __PS3__
#if !(0 || defined __PS3__ || 0 || 0)
DWORD XGetLanguage() { return 1; }
DWORD XGetLocale() { return 0; }
DWORD XEnableGuestSignin(BOOL fEnable) { return 0; }
#endif



/////////////////////////////////////////////// Profile library

/////////////////////////////////////////////////////// Sentient manager

HRESULT CSentientManager::Init() { return S_OK; }
HRESULT CSentientManager::Tick() { return S_OK; }
HRESULT CSentientManager::Flush() { return S_OK; }
BOOL CSentientManager::RecordPlayerSessionStart(DWORD dwUserId) { return true; }
BOOL CSentientManager::RecordPlayerSessionExit(DWORD dwUserId, int exitStatus) { return true; }
BOOL CSentientManager::RecordHeartBeat(DWORD dwUserId) { return true; }
BOOL CSentientManager::RecordLevelStart(DWORD dwUserId, ESen_FriendOrMatch friendsOrMatch, ESen_CompeteOrCoop competeOrCoop, int difficulty, DWORD numberOfLocalPlayers, DWORD numberOfOnlinePlayers) { return true; }
BOOL CSentientManager::RecordLevelExit(DWORD dwUserId, ESen_LevelExitStatus levelExitStatus) { return true; }
BOOL CSentientManager::RecordLevelSaveOrCheckpoint(DWORD dwUserId, INT saveOrCheckPointID, INT saveSizeInBytes) { return true; }
BOOL CSentientManager::RecordLevelResume(DWORD dwUserId, ESen_FriendOrMatch friendsOrMatch, ESen_CompeteOrCoop competeOrCoop, int difficulty, DWORD numberOfLocalPlayers, DWORD numberOfOnlinePlayers, INT saveOrCheckPointID)  { return true; }
BOOL CSentientManager::RecordPauseOrInactive(DWORD dwUserId)  { return true; }
BOOL CSentientManager::RecordUnpauseOrActive(DWORD dwUserId) { return true; }
BOOL CSentientManager::RecordMenuShown(DWORD dwUserId, INT menuID, INT optionalMenuSubID) { return true; }
BOOL CSentientManager::RecordAchievementUnlocked(DWORD dwUserId, INT achievementID, INT achievementGamerscore) { return true; }
BOOL CSentientManager::RecordMediaShareUpload(DWORD dwUserId, ESen_MediaDestination mediaDestination, ESen_MediaType mediaType) { return true; }
BOOL CSentientManager::RecordUpsellPresented(DWORD dwUserId, ESen_UpsellID upsellId, INT marketplaceOfferID) { return true; }
BOOL CSentientManager::RecordUpsellResponded(DWORD dwUserId, ESen_UpsellID upsellId, INT marketplaceOfferID, ESen_UpsellOutcome upsellOutcome) { return true; }
BOOL CSentientManager::RecordPlayerDiedOrFailed(DWORD dwUserId, INT lowResMapX, INT lowResMapY, INT lowResMapZ, INT mapID, INT playerWeaponID, INT enemyWeaponID, ETelemetryChallenges enemyTypeID) { return true; }
BOOL CSentientManager::RecordEnemyKilledOrOvercome(DWORD dwUserId, INT lowResMapX, INT lowResMapY, INT lowResMapZ, INT mapID, INT playerWeaponID, INT enemyWeaponID, ETelemetryChallenges enemyTypeID) { return true; }
BOOL CSentientManager::RecordSkinChanged(DWORD dwUserId, DWORD dwSkinId) { return true; }
BOOL CSentientManager::RecordBanLevel(DWORD dwUserId) { return true; }
BOOL CSentientManager::RecordUnBanLevel(DWORD dwUserId) { return true; }
INT CSentientManager::GetMultiplayerInstanceID() { return 0; }
INT CSentientManager::GenerateMultiplayerInstanceId() { return 0; }
void CSentientManager::SetMultiplayerInstanceId(INT value) {}

////////////////////////////////////////////////////////  Stats counter

/*
StatsCounter::StatsCounter() {}
void StatsCounter::award(Stat *stat, unsigned int difficulty, unsigned int count) {}
bool StatsCounter::hasTaken(Achievement *ach) { return true; }
bool StatsCounter::canTake(Achievement *ach) { return true; }
unsigned int StatsCounter::getValue(Stat *stat, unsigned int difficulty) { return 0; }
unsigned int StatsCounter::getTotalValue(Stat *stat) { return 0; }
void StatsCounter::tick(int player) {}
void StatsCounter::parse(void* data) {}
void StatsCounter::clear() {}
void StatsCounter::save(int player, bool force) {}
void StatsCounter::flushLeaderboards() {}
void StatsCounter::saveLeaderboards() {}
void StatsCounter::setupStatBoards() {}
#ifdef _DEBUG
void StatsCounter::WipeLeaderboards() {}
#endif
*/
