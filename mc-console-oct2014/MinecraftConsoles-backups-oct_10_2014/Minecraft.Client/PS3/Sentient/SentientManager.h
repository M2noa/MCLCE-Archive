#pragma once
#include "MinecraftTelemetry.h"

class CSentientManager
{
public:
	enum ETelemetryEvent
	{
		eTelemetry_PlayerSessionStart,
		eTelemetry_PlayerSessionExit,
		eTelemetry_HeartBeat,
		eTelemetry_LevelStart,
		eTelemetry_LevelExit,
		eTelemetry_LevelSaveOrCheckpoint,
		eTelemetry_PauseOrInactive,
		eTelemetry_UnpauseOrActive,
		eTelemetry_MenuShown,
		eTelemetry_AchievementUnlocked,
		eTelemetry_MediaShareUpload,
		eTelemetry_UpsellPresented,
		eTelemetry_UpsellResponded,
		eTelemetry_PlayerDiedOrFailed,
		eTelemetry_EnemyKilledOrOvercome,
	};

	HRESULT Init();
	HRESULT Tick();

	HRESULT Flush();

	BOOL RecordPlayerSessionStart(DWORD dwUserId);
	BOOL RecordPlayerSessionExit(DWORD dwUserId, int exitStatus);
	BOOL RecordHeartBeat(DWORD dwUserId);
	BOOL RecordLevelStart(DWORD dwUserId, ESen_FriendOrMatch friendsOrMatch, ESen_CompeteOrCoop competeOrCoop, int difficulty, DWORD numberOfLocalPlayers, DWORD numberOfOnlinePlayers);
	BOOL RecordLevelExit(DWORD dwUserId, ESen_LevelExitStatus levelExitStatus);
	BOOL RecordLevelSaveOrCheckpoint(DWORD dwUserId, INT saveOrCheckPointID, INT saveSizeInBytes);
	BOOL RecordLevelResume(DWORD dwUserId, ESen_FriendOrMatch friendsOrMatch, ESen_CompeteOrCoop competeOrCoop, int difficulty, DWORD numberOfLocalPlayers, DWORD numberOfOnlinePlayers, INT saveOrCheckPointID);
	BOOL RecordPauseOrInactive(DWORD dwUserId);
	BOOL RecordUnpauseOrActive(DWORD dwUserId);
	BOOL RecordMenuShown(DWORD dwUserId, INT menuID, INT optionalMenuSubID);
	BOOL RecordAchievementUnlocked(DWORD dwUserId, INT achievementID, INT achievementGamerscore);
	BOOL RecordMediaShareUpload(DWORD dwUserId, ESen_MediaDestination mediaDestination, ESen_MediaType mediaType);
	BOOL RecordUpsellPresented(DWORD dwUserId, ESen_UpsellID upsellId, INT marketplaceOfferID);
	BOOL RecordUpsellResponded(DWORD dwUserId, ESen_UpsellID upsellId, INT marketplaceOfferID, ESen_UpsellOutcome upsellOutcome);
	BOOL RecordPlayerDiedOrFailed(DWORD dwUserId, INT lowResMapX, INT lowResMapY, INT lowResMapZ, INT mapID, INT playerWeaponID, INT enemyWeaponID, ETelemetryChallenges enemyTypeID);
	BOOL RecordEnemyKilledOrOvercome(DWORD dwUserId, INT lowResMapX, INT lowResMapY, INT lowResMapZ, INT mapID, INT playerWeaponID, INT enemyWeaponID, ETelemetryChallenges enemyTypeID);

	BOOL RecordSkinChanged(DWORD dwUserId, DWORD dwSkinId);
	BOOL RecordBanLevel(DWORD dwUserId);
	BOOL RecordUnBanLevel(DWORD dwUserId);

	INT GetMultiplayerInstanceID();
	INT GenerateMultiplayerInstanceId();
	void SetMultiplayerInstanceId(INT value);

private:
	float m_initialiseTime;
	float m_lastHeartbeat;
	bool m_bFirstFlush;

	float m_fLevelStartTime[XUSER_MAX_COUNT];

	INT m_multiplayerInstanceID;
	DWORD m_levelInstanceID;

	// Helper functions to get the various common settings
	INT GetSecondsSinceInitialize();
	INT GetMode(DWORD dwUserId);
	INT GetSubMode(DWORD dwUserId);
	INT GetLevelId(DWORD dwUserId);
	INT GetSubLevelId(DWORD dwUserId);
	INT GetTitleBuildId();
	INT GetLevelInstanceID();
	INT GetSingleOrMultiplayer();
	INT GetDifficultyLevel(INT diff);
	INT GetLicense();
	INT GetDefaultGameControls();
	INT GetAudioSettings(DWORD dwUserId);
	INT GetLevelExitProgressStat1();
	INT GetLevelExitProgressStat2();
};

extern CSentientManager SentientManager;