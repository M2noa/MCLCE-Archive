#pragma once
// 4J Stu - Enums as defined by the common Sentient telemetry format

//##################################
// DO NOT CHANGE ANY OF THESE VALUES
//##################################


/*************************************
  AudioSettings
 *************************************
 Are players changing default audio settings?
 */
enum ESen_AudioSettings
{
	eSen_AudioSettings_Undefined = 0,
	eSen_AudioSettings_Off = 1,
	eSen_AudioSettings_On_Default = 2,
	eSen_AudioSettings_On_CustomSetting = 3,
};

/*************************************
  CompeteOrCoop
 *************************************
 Indicates whether players are playing a cooperative mode or a competitive mode.
 */
enum ESen_CompeteOrCoop
{
	eSen_CompeteOrCoop_Undefined = 0,
	eSen_CompeteOrCoop_Cooperative = 1,
	eSen_CompeteOrCoop_Competitive = 2,
	eSen_CompeteOrCoop_Coop_and_Competitive = 3,
};

/*************************************
  DefaultGameControls
 *************************************
 This is intended to capture whether players played using default control scheme or customized the control scheme. 
 */
enum ESen_DefaultGameControls
{
	eSen_DefaultGameControls_Undefined = 0,
	eSen_DefaultGameControls_Default_controls = 1,
	eSen_DefaultGameControls_Custom_controls = 2,
};

/*************************************
  DifficultyLevel
 *************************************
 An in-game setting that differentiates the challenge imposed on the user.  Normalized to a standard 5-point scale.
 */
enum ESen_DifficultyLevel
{
	eSen_DifficultyLevel_Undefined = 0,
	eSen_DifficultyLevel_Easiest = 1,
	eSen_DifficultyLevel_Easier = 2,
	eSen_DifficultyLevel_Normal = 3,
	eSen_DifficultyLevel_Harder = 4,
	eSen_DifficultyLevel_Hardest = 5,
};

/*************************************
  GameInputType
 *************************************
 Used to determine the different modes of input used in the game. For gamepad/keyboard/mouse usage, it is not necessary to call this for every single input.
 Also, if polling is used, calling this event occasionally may also work.
 */
enum ESen_GameInputType
{
	eSen_GameInputType_Undefined = 0,
	eSen_GameInputType_Xbox_Controller = 1,
	eSen_GameInputType_Gesture = 2,
	eSen_GameInputType_Voice = 3,
	eSen_GameInputType_Voice_and_Gesture_Together = 4,
	eSen_GameInputType_Touch = 5,
	eSen_GameInputType_Keyboard = 6,
	eSen_GameInputType_Mouse = 7,
};

/*************************************
  LevelExitStatus
 *************************************
 Indicates whether the player successfully completed the level.  Critical for understanding the difficulty of a game with checkpoints or saves.
 */
enum ESen_LevelExitStatus
{
	eSen_LevelExitStatus_Undefined = 0,
	eSen_LevelExitStatus_Exited = 1,
	eSen_LevelExitStatus_Succeeded = 2,
	eSen_LevelExitStatus_Failed = 3,
};

/*************************************
  License
 *************************************
 Differentiates trial/demo from full purchased titles
 */
enum ESen_License
{
	eSen_License_Undefined = 0,
	eSen_License_Trial_or_Demo = 1,
	eSen_License_Full_Purchased_Title = 2,
};

/*************************************
  MediaDestination
 *************************************
 Tracks where media is uploaded to (like facebook)
 */
enum ESen_MediaDestination
{
	ESen_MediaDestination_Undefined = 0,
	ESen_MediaDestination_Kinect_Share = 1,
	ESen_MediaDestination_Facebook = 2,
	ESen_MediaDestination_YouTube = 3,
	ESen_MediaDestination_Other = 4
};

/*************************************
  MediaType
 *************************************
 Used to capture the type of media players are uploading to KinectShare 
 */
enum ESen_MediaType
{
	eSen_MediaType_Undefined = 0,
	eSen_MediaType_Picture = 1,
	eSen_MediaType_Video = 2,
	eSen_MediaType_Other_UGC = 3,
};
 
/*************************************
  SingleOrMultiplayer
 *************************************
 Indicates whether the game is being played in single or multiplayer mode and whether multiplayer is being played locally or over live.
 */
enum ESen_SingleOrMultiplayer
{
	eSen_SingleOrMultiplayer_Undefined = 0,
	eSen_SingleOrMultiplayer_Single_Player = 1,
	eSen_SingleOrMultiplayer_Multiplayer_Local = 2,
	eSen_SingleOrMultiplayer_Multiplayer_Live = 3,
	eSen_SingleOrMultiplayer_Multiplayer_Both_Local_and_Live = 4,
};

/*************************************
  FriendOrMatch
 *************************************
 Are players playing with friends or were they matched?
 */
enum ESen_FriendOrMatch
{
	eSen_FriendOrMatch_Undefined = 0, // (use if a single player game)
	eSen_FriendOrMatch_Playing_With_Invited_Friends = 1,
	eSen_FriendOrMatch_Playing_With_Match_Made_Opponents = 2,
	eSen_FriendOrMatch_Playing_With_Both_Friends_And_Matched_Opponents = 3,
	eSen_FriendOrMatch_Joined_Through_An_Xbox_Live_Party = 4,
	eSen_FriendOrMatch_Joined_Through_An_In_Game_Party = 5,
};

/*************************************
  UpsellID
 *************************************
 Which upsell has been presented?
 */
enum ESen_UpsellID
{
	eSen_UpsellID_Undefined = 0,
	eSen_UpsellID_Full_Version_Of_Game = 1,

	// Added TU3
	eSet_UpsellID_Skin_DLC = 2,
	eSet_UpsellID_Texture_DLC = 3,

	//2-max= Up to game
};

/*************************************
  UpsellOutcome
 *************************************
 What was the outcome of the upsell?
 */
enum ESen_UpsellOutcome
{
	eSen_UpsellOutcome_Undefined = 0,
	eSen_UpsellOutcome_Accepted = 1,
	eSen_UpsellOutcome_Declined = 2,
	eSen_UpsellOutcome_Went_To_Guide = 3,
	eSen_UpsellOutcome_Other = 4,
};
