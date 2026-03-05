#pragma once
#include "MinecraftTelemetry.h"

class CSentientManager : public CTelemetryManager
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

	virtual HRESULT Init();
	virtual HRESULT Tick();

	virtual HRESULT Flush();

	virtual BOOL RecordPlayerSessionStart(DWORD dwUserId);
	virtual BOOL RecordPlayerSessionExit(DWORD dwUserId, int exitStatus);
	virtual BOOL RecordHeartBeat(DWORD dwUserId);
	virtual BOOL RecordLevelStart(DWORD dwUserId, ESen_FriendOrMatch friendsOrMatch, ESen_CompeteOrCoop competeOrCoop, int difficulty, DWORD numberOfLocalPlayers, DWORD numberOfOnlinePlayers);
	virtual BOOL RecordLevelExit(DWORD dwUserId, ESen_LevelExitStatus levelExitStatus);
	virtual BOOL RecordLevelSaveOrCheckpoint(DWORD dwUserId, INT saveOrCheckPointID, INT saveSizeInBytes);
	virtual BOOL RecordLevelResume(DWORD dwUserId, ESen_FriendOrMatch friendsOrMatch, ESen_CompeteOrCoop competeOrCoop, int difficulty, DWORD numberOfLocalPlayers, DWORD numberOfOnlinePlayers, INT saveOrCheckPointID);
	virtual BOOL RecordPauseOrInactive(DWORD dwUserId);
	virtual BOOL RecordUnpauseOrActive(DWORD dwUserId);
	virtual BOOL RecordMenuShown(DWORD dwUserId, INT menuID, INT optionalMenuSubID);
	virtual BOOL RecordAchievementUnlocked(DWORD dwUserId, INT achievementID, INT achievementGamerscore);
	virtual BOOL RecordMediaShareUpload(DWORD dwUserId, ESen_MediaDestination mediaDestination, ESen_MediaType mediaType);
	virtual BOOL RecordUpsellPresented(DWORD dwUserId, ESen_UpsellID upsellId, INT marketplaceOfferID);
	virtual BOOL RecordUpsellResponded(DWORD dwUserId, ESen_UpsellID upsellId, INT marketplaceOfferID, ESen_UpsellOutcome upsellOutcome);
	virtual BOOL RecordPlayerDiedOrFailed(DWORD dwUserId, INT lowResMapX, INT lowResMapY, INT lowResMapZ, INT mapID, INT playerWeaponID, INT enemyWeaponID, ETelemetryChallenges enemyTypeID);
	virtual BOOL RecordEnemyKilledOrOvercome(DWORD dwUserId, INT lowResMapX, INT lowResMapY, INT lowResMapZ, INT mapID, INT playerWeaponID, INT enemyWeaponID, ETelemetryChallenges enemyTypeID);
	virtual BOOL RecordTexturePackLoaded(DWORD dwUserId, INT texturePackId, INT purchased);

	virtual BOOL RecordSkinChanged(DWORD dwUserId, DWORD dwSkinId);
	virtual BOOL RecordBanLevel(DWORD dwUserId);
	virtual BOOL RecordUnBanLevel(DWORD dwUserId);

	virtual INT GetMultiplayerInstanceID();
	virtual INT GenerateMultiplayerInstanceId();
	virtual void SetMultiplayerInstanceId(INT value);

private:
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