#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cell/sysmodule.h>
#include <sys/process.h>
#include <sysutil/sysutil_avc2.h>               /* for using AV Chat2 Utility       */
#include <np.h>                         /* for using NP Matching            */
#include <netex/net.h>
#include <netex/libnetctl.h>
#include <sysutil/sysutil_sysparam.h>


#define AVC2_PARAM_DEFAULT_MAX_PLAYERS          (8)
#define AVC2_PARAM_DEFAULT_MAX_SPEAKERS         (4)

/* state list */
enum EAVCState
{
	AVC_STATE_IDLE = 0,			

	AVC_STATE_CHAT_INIT,				
	AVC_STATE_CHAT_LOAD,		
	AVC_STATE_CHAT_JOIN,
	AVC_STATE_CHAT_SESSION_PROCESSING,
	AVC_STATE_CHAT_LEAVE,
	AVC_STATE_CHAT_RESET,
	AVC_STATE_CHAT_UNLOAD,	

	AVC_STATE_EXIT,			
};
/* event list */
enum EAVCEvent
{
	AVC_EVENT_NON = AVC_STATE_EXIT + 1,

	AVC_EVENT_EPSILON,
	AVC_EVENT_LAN_DISCONNECT,
	AVC_EVENT_ONLINE,
	AVC_EVENT_OFFLINE,
	AVC_EVENT_EXIT_GAME,
	AVC_EVENT_ROOM_CREATE,
	AVC_EVENT_ROOM_SEARCH,
	AVC_EVENT_ERROR,
	AVC_EVENT_FATAL_ERROR,
	AVC_EVENT_NETDIALOG_FINISHED,
	AVC_EVENT_NETDIALOG_UNLOADED,
	AVC_EVENT_NP2_INIT_SUCCEEDED,
	AVC_EVENT_NP2_FINALIZE_SUCCEEDED,
	AVC_EVENT_NP2_START_CONTEXT_SUCCEEDED,
	AVC_EVENT_NP2_STOP_CONTEXT_SUCCEEDED,
	AVC_EVENT_NP2_ROOM_DESTROY_OR_KICKEDOUT,
	AVC_EVENT_NP2_ROOM_MEMBER_LEFT,
	AVC_EVENT_NP2_ROOM_MEMBER_JOINED,
	AVC_EVENT_CHAT_LOAD_SUCCEEDED,
	AVC_EVENT_CHAT_LOAD_FAILED,
	AVC_EVENT_CHAT_JOIN_SUCCEEDED,
	AVC_EVENT_CHAT_JOIN_FAILED,
	AVC_EVENT_CHAT_LEAVE_SUCCEEDED,
	AVC_EVENT_CHAT_LEAVE_FAILED,
	AVC_EVENT_CHAT_UNLOAD_SUCCEEDED,
	AVC_EVENT_CHAT_UNLOAD_FAILED,
	AVC_EVENT_CHAT_INIT_SUCCEEDED,
	AVC_EVENT_CHAT_FINALIZE_SUCCEEDED,
	AVC_EVENT_CHAT_SESSION_STOPPED,
	AVC_EVENT_CREATE_JOIN_ROOM_SUCCEEDED,
	AVC_EVENT_SEARCH_ROOM_SUCCEEDED,
	AVC_EVENT_JOIN_ROOM_SUCCEEDED,
	AVC_EVENT_LEAVE_ROOM_SUCCEEDED,
	AVC_EVENT_SIGNALING_ESTABLISHED,
	AVC_EVENT_SIGNALING_DEAD,
	AVC_EVENT_UI_CLOSE_SUCCEEDED,
};


typedef struct
{
	int state;
	int	event;
	int (*func)(void);
	EAVCState	new_state;
} state_transition_table;


class SonyVoiceChat
{
public:

	static void init(SQRNetworkManager_PS3* pNetMan);
	static void shutdown();
	static void tick();
	static void setEnabled(bool bEnabled);
	static bool hasMicConnected(const SceNpMatching2RoomMemberId *players_id);
	static bool isTalking(SceNpMatching2RoomMemberId* players_id);
	static void mute(bool bMute);			//Turn chat audio on or off
	static void mutePlayer(const SceNpMatching2RoomMemberId member_id, bool bMute);		//Turn chat audio from a specified player on or off;
	static void muteLocalPlayer(bool bMute); //Turn microphone input on or off;

	static bool isMuted();
	static bool isMutedPlayer(const SceNpMatching2RoomMemberId member_id);
	static bool isMutedLocalPlayer(); //Turn microphone input on or off;

	static void setVolume(float vol);		// 0.0f - 1.0f range
	static float getVolume();				// 0.0f - 1.0f range

	static bool isShuttingDown() { return (sm_state == AVC_STATE_CHAT_LEAVE || sm_state == AVC_STATE_CHAT_UNLOAD || AVC_STATE_CHAT_RESET); }
	static void signalRoomDestroyed()		{ if(!isShuttingDown()) sm_event = AVC_EVENT_NP2_ROOM_DESTROY_OR_KICKEDOUT;}
	static void signalRoomKickedOut()		{ if(!isShuttingDown()) sm_event = AVC_EVENT_NP2_ROOM_DESTROY_OR_KICKEDOUT;}
	static void signalDisconnected()		{ if(!isShuttingDown()) sm_event = AVC_EVENT_LAN_DISCONNECT;}
private:
// 	static np2_matching2_info				sm_matchingInfo;
	static EAVCEvent						sm_event;
	static EAVCState						sm_state;
	static SQRNetworkManager_PS3*				sm_pNetworkManager;
	static bool								sm_bEnabled;
	static uint8_t							sm_micStatus;
	static bool								sm_bLoaded;
	static bool								sm_bUnloading;
	static unordered_map<SceNpMatching2RoomMemberId, bool> sm_bTalkingMap;
	static bool								sm_bCanStart;		// set to true on init, false on disconnect, used to see if we should start after a re-enable
	static bool								sm_isChatRestricted;	// true if the parental controls have been set on the main users account
	static int								sm_currentBitrate;
	static int eventcb_load(CellSysutilAvc2EventId event_id, CellSysutilAvc2EventParam event_param, void *userdata);
	static int eventcb_join(CellSysutilAvc2EventId event_id, CellSysutilAvc2EventParam event_param, void *userdata);
	static int eventcb_leave( CellSysutilAvc2EventId event_id,	CellSysutilAvc2EventParam event_param, void *userdata);
	static int eventcb_unload(CellSysutilAvc2EventId event_id,	CellSysutilAvc2EventParam event_param, void *userdata);
	static int eventcb_voiceDetected(CellSysutilAvc2EventId event_id,	CellSysutilAvc2EventParam event_param, void *userdata);
	static void eventcb( CellSysutilAvc2EventId event_id, CellSysutilAvc2EventParam event_param, void *userdata);

	static int initialize();
	static int finalize();

	static int load();
	static int unload();

	static int join();
	static int leave();

	static int start();
	static int stop();

	static int startStream();
	static int stopStream();
// 	static int update_target();

	static void do_state_transition( state_transition_table *tbl, int tbl_size);
	static int invoke_epsilon_event();

	static void setBitRate();

	static void setEvent(EAVCEvent event);
	static void setState(EAVCState state);
	static void printStateAndEvent();


};