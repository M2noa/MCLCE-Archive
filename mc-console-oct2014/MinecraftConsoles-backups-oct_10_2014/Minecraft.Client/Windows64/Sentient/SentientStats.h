/************************************************************************/
/* THIS FILE WAS AUTOMATICALLY GENERATED                                */
/* PLEASE DO NOT MODIFY                                                 */
/************************************************************************/
// Generated from Version: 20, on (6/19/2012 9:21:23 AM)

#pragma once

/************************************************************************/
/* STATS                                                                */
/************************************************************************/

// PlayerSessionStart
// Player signed in or joined
BOOL	SenStatPlayerSessionStart              	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT TitleBuildID, INT SkeletonDistanceInInches, INT EnrollmentType, INT NumberOfSkeletonsInView );

// PlayerSessionExit
// Player signed out or left
BOOL	SenStatPlayerSessionExit               	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID );

// HeartBeat
// Sent every 60 seconds by title
BOOL	SenStatHeartBeat                       	( DWORD dwUserID, INT SecondsSinceInitialize );

// LevelStart
// Level started
BOOL	SenStatLevelStart                      	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID, INT SingleOrMultiplayer, INT FriendsOrMatch, INT CompeteOrCoop, INT DifficultyLevel, INT NumberOfLocalPlayers, INT NumberOfOnlinePlayers, INT License, INT DefaultGameControls, INT AudioSettings, INT SkeletonDistanceInInches, INT NumberOfSkeletonsInView );

// LevelExit
// Level exited
BOOL	SenStatLevelExit                       	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID, INT LevelExitStatus, INT LevelExitProgressStat1, INT LevelExitProgressStat2, INT LevelDurationInSeconds );

// LevelSaveOrCheckpoint
// Level saved explicitly or implicitly
BOOL	SenStatLevelSaveOrCheckpoint           	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID, INT LevelExitProgressStat1, INT LevelExitProgressStat2, INT LevelDurationInSeconds, INT SaveOrCheckPointID );

// LevelResume
// Level resumed from a save or restarted at a checkpoint
BOOL	SenStatLevelResume                     	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID, INT SingleOrMultiplayer, INT FriendsOrMatch, INT CompeteOrCoop, INT DifficultyLevel, INT NumberOfLocalPlayers, INT NumberOfOnlinePlayers, INT License, INT DefaultGameControls, INT SaveOrCheckPointID, INT AudioSettings, INT SkeletonDistanceInInches, INT NumberOfSkeletonsInView );

// PauseOrInactive
// Player paused game or has become inactive, level and mode are for what the player is leaving
BOOL	SenStatPauseOrInactive                 	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID );

// UnpauseOrActive
// Player unpaused game or has become active, level and mode are for what the player is entering into
BOOL	SenStatUnpauseOrActive                 	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID );

// MenuShown
// A menu screen or major menu area has been shown
BOOL	SenStatMenuShown                       	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT MenuID, INT OptionalMenuSubID, INT LevelInstanceID, INT MultiplayerInstanceID );

// AchievementUnlocked
// An achievement was unlocked
BOOL	SenStatAchievementUnlocked             	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID, INT AchievementID, INT AchievementGamerscore );

// MediaShareUpload
// The user uploaded something to Kinect Share
BOOL	SenStatMediaShareUpload                	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID, INT MediaDestination, INT MediaType );

// UpsellPresented
// The user is shown an upsell to purchase something
BOOL	SenStatUpsellPresented                 	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID, INT UpsellID, INT MarketplaceOfferID );

// UpsellResponded
// The user responded to the upsell
BOOL	SenStatUpsellResponded                 	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID, INT UpsellID, INT MarketplaceOfferID, INT UpsellOutcome );

// PlayerDiedOrFailed
// The player died or failed a challenge - can be used for many types of failure
BOOL	SenStatPlayerDiedOrFailed              	( DWORD dwUserID, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID, INT LowResMapX, INT LowResMapY, INT LowResMapZ, INT MapID, INT PlayerWeaponID, INT EnemyWeaponID, INT EnemyTypeID, INT SecondsSinceInitialize, INT CopyOfSecondsSinceInitialize );

// EnemyKilledOrOvercome
// The player killed an enemy or overcame or solved a major challenge
BOOL	SenStatEnemyKilledOrOvercome           	( DWORD dwUserID, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID, INT LowResMapX, INT LowResMapY, INT LowResMapZ, INT MapID, INT PlayerWeaponID, INT EnemyWeaponID, INT EnemyTypeID, INT SecondsSinceInitialize, INT CopyOfSecondsSinceInitialize );

// SkinChanged
// The player has changed their skin, level and mode are for what the player is currently in
BOOL	SenStatSkinChanged                     	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID, INT SkinID );

// BanLevel
// The player has banned a level, level and mode are for what the player is currently in and banning
BOOL	SenStatBanLevel                        	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID );

// UnBanLevel
// The player has ubbanned a level, level and mode are for what the player is currently in and unbanning
BOOL	SenStatUnBanLevel                      	( DWORD dwUserID, INT SecondsSinceInitialize, INT ModeID, INT OptionalSubModeID, INT LevelID, INT OptionalSubLevelID, INT LevelInstanceID, INT MultiplayerInstanceID );

