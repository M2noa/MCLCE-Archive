


#pragma once


// Note - there are now 3 types of PlayerUID
// (1) A full online ID - either the primary login, or a sub-signin through to PSN. This has m_onlineID set up as a normal SceNpOnlineId, with dummy[0] set to 0
// (2) An offline ID, where there is also a primary login on the system. This has m_onlineID set up to copy the primary SceNpOnlineId, except with dummy[0] set to the controller ID of this other player
// (3) An offline ID, where there isn't a primary PSN login on the system. This has SceNpOnlineId fully zeroed.

class PlayerUID
{
	char m_onlineID[SCE_NET_NP_ONLINEID_MAX_LENGTH];
	char term;
	bool m_bSignedIntoPSN : 1;
	unsigned char m_quadrant : 2;
	uint8_t m_macAddress[CELL_NET_CTL_ETHER_ADDR_LEN];
	CellSysutilUserId	m_userID;		// user logged on to the XMB

public:

	class Hash 
	{
	public:		
		std::size_t operator()(const PlayerUID& k) const;
	};

	PlayerUID();
	PlayerUID(CellSysutilUserId userID, SceNpOnlineId& onlineID, bool bSignedInPSN, int quadrant);
	PlayerUID(std::wstring fromString);

	bool operator==(const PlayerUID& rhs) const;
	bool operator!=(const PlayerUID& rhs);
	void setCurrentMacAddress();
	std::wstring macAddressStr() const;
	std::wstring userIDStr() const;
	std::wstring toString() const;
	void setOnlineID(SceNpOnlineId& id, bool bSignedIntoPSN);
	void setUserID(unsigned int id);


	const char*			getOnlineID() const		{ return m_onlineID; }
	CellSysutilUserId	getUserID() const		{ return m_userID; }
	int					getQuadrant() const		{ return m_quadrant;  }
	bool				isPrimaryUser() const;	// only true if we're on the local machine and signed into the first quadrant;
	bool				isSignedIntoPSN() const	{ return m_bSignedIntoPSN; }
private:
};





class GameSessionUID
{
	char m_onlineID[SCE_NET_NP_ONLINEID_MAX_LENGTH];
	char term;
	bool m_bSignedIntoPSN : 1;
	unsigned char m_quadrant : 2;
public:
	GameSessionUID();
	GameSessionUID(int nullVal);

	bool operator==(const GameSessionUID& rhs) const;
	bool operator!=(const GameSessionUID& rhs);
	GameSessionUID& operator=(const PlayerUID& rhs);

	const char*			getOnlineID() const		{ return m_onlineID; }
	int					getQuadrant() const		{ return m_quadrant;  }
	bool				isSignedIntoPSN() const	{ return m_bSignedIntoPSN; }
};
