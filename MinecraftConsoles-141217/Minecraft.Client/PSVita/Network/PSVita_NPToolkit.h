#pragma once

#include <np_toolkit.h>
// #include <np_toolkit/game_custom_data_interface.h>


class PSVitaNPToolkit
{
public:
	static void init();
	static void sceNpToolkitCallback( const sce::Toolkit::NP::Event& event);
	static void coreCallback( const sce::Toolkit::NP::Event& event);
	static void presenceCallback( const sce::Toolkit::NP::Event& event);
	static void profileCallback( const sce::Toolkit::NP::Event& event);
	static void messagingCallback( const sce::Toolkit::NP::Event& event);
	static void sessionsCallback( const sce::Toolkit::NP::Event& event);
	static void gameCustomDataCallback( const sce::Toolkit::NP::Event& event);

// 	static void createNPSession();
// 	static void destroyNPSession();
// 	static void joinNPSession();
// 	static void leaveNPSession();
// 	static SceNpSessionId* getNPSessionID() { return &m_currentSessionInfo.npSessionId; }

 	static void getMessageData(SceAppUtilAppEventParam* paramData);
private:
// 	static sce::Toolkit::NP::Utilities::Future<sce::Toolkit::NP::NpSessionInformation> sm_createJoinFuture;
//  	static sce::Toolkit::NP::NpSessionInformation m_currentSessionInfo;
	static sce::Toolkit::NP::Utilities::Future<sce::Toolkit::NP::MessageAttachment> m_messageData;

};

