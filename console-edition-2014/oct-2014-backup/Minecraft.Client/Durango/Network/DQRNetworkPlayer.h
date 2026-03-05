#pragma once
#include "DQRNetworkManager.h"
#include <queue>

// This is the lowest level class for handling the concept of a player on Durango. This is managed by DQRNetworkManager. The game shouldn't directly communicate
// with this class, as it is wrapped by NetworkPlayerDurango which is an implementation of a platform-independent interface INetworkPlayer.

class DQRNetworkPlayer
{
public:
	friend class DQRNetworkManager;

	typedef enum
	{
		DNP_TYPE_HOST,		// This player represents the host
		DNP_TYPE_LOCAL,		// On host - this player is a local player that needs communicated with specially not using rudp. On clients - this is a local player, where network communications can be used to communicate with the host
		DNP_TYPE_REMOTE,	// On host - this player can be used to communicate from between the host and this player. On clients - this is a remote player that cannot be communicated with
	} eDQRNetworkPlayerType;

	DQRNetworkPlayer();
	DQRNetworkPlayer(DQRNetworkManager *manager, eDQRNetworkPlayerType playerType, bool onHost, int localPlayerIdx, unsigned int sessionAddress);
	~DQRNetworkPlayer();

	PlayerUID					GetUID();
	void						SetUID(PlayerUID UID);
	int							GetLocalPlayerIndex();
	uintptr_t					GetCustomDataValue();
	void						SetCustomDataValue(uintptr_t data);
	bool						IsRemote();
	bool						IsHost();
	bool						IsLocal();
	bool						IsSameSystem(DQRNetworkPlayer *other);
	bool						HasVoice();
	bool						IsTalking();
	bool						HasCamera();
	LPCWSTR						GetGamertag();
	int							GetSmallId();
	void						SetSmallId(unsigned char smallId);
	int							GetSessionIndex();
	void						SendData( DQRNetworkPlayer *pPlayerTarget, const void *data, unsigned int dataSize );

	int							GetSendQueueSizeBytes();
	int							GetSendQueueSizeMessages();

	wchar_t						*GetName();
	void						SetName(const wchar_t *name);
		
	std::wstring				GetDisplayName();
	void						SetDisplayName(std::wstring displayName);
private:
	void						SendInternal(const void *data, unsigned int dataSize);

	eDQRNetworkPlayerType		m_type;					// The player type
	bool						m_host;					// Whether this actual player class is stored on a host (not whether it represents the host, or a player on the host machine)
	int							m_localPlayerIdx;		// Index of this player on the machine to which it belongs
	DQRNetworkManager			*m_manager;				// Pointer back to the manager that is managing this player

	PlayerUID					m_UID;
	uintptr_t					m_customData;
	unsigned char				m_smallId;
	unsigned int				m_sessionAddress;

	wchar_t						m_name[21];
	std::wstring				m_displayName;
};