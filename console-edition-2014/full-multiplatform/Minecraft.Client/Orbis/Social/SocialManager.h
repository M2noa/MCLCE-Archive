//
// Class to handle and manage integration with social networks.
// 4J Studios Ltd, 2011.
// Andy West
//

#ifndef _SOCIAL_MANAGER_H
#define _SOCIAL_MANAGER_H

#include <xsocialpost.h>

#define MAX_SOCIALPOST_CAPTION	60
#define MAX_SOCIALPOST_DESC		100

// XDK only provides for facebook so far. Others may follow!?
enum ESocialNetwork
{
	eFacebook = 0,
	eNumSocialNetworks
};


// Class follows singleton design pattern.
class CSocialManager
{
private:
	// Default constructor, copy constructor and assignment operator are all private.
	CSocialManager();
	CSocialManager( const CSocialManager& );
	CSocialManager& operator= ( const CSocialManager& );

	// Static private instance.
	static CSocialManager* m_pInstance;

	// Bitset of title posting capability flags ( XSOCIAL_CAPABILITY_POSTIMAGE, XSOCIAL_CAPABILITY_POSTLINK ).
	DWORD		m_dwSocialPostingCapability;

	// Index of user who made current active request.
	DWORD		m_dwCurrRequestUser;

	// WESTY : Not sure if we even need to get social access key!
/*
	// Size of the social network access key text buffer.
	DWORD		m_dwAccessKeyTextSize;

	// Pointer to the social network access key text buffer.
	LPWSTR		m_pAccessKeyText;
	*/

	// The various states of the manager.
	enum EState
	{
		eStateUnitialised = 0,
		eStateReady,
		eStateGetPostingCapability,
		eStatePostingImage,
		eStatePostingLink,
	};

	
	// Current state that manager is in.
	EState					m_eCurrState;

	// For xsocial asyncronous operations.
	XOVERLAPPED				m_Overlapped;
	DWORD					m_dwOverlappedResultCode;

	// Social post preview image struct.
	XSOCIAL_PREVIEWIMAGE	m_PostPreviewImage;

#ifdef _XBOX
	// Social post image params.
	XSOCIAL_IMAGEPOSTPARAMS	m_PostImageParams;

	// Social post link params.
	XSOCIAL_LINKPOSTPARAMS	m_PostLinkParams;
#endif

	// Image details for posting an image to social network.
	unsigned char*			m_pMainImageBuffer;
	DWORD					m_dwMainImageBufferSize;

	void	DestroyMainPostImage();
	void    DestroyPreviewPostImage();

	// WESTY : Not sure if we even need to get social access key!
/*
	bool	GetSocialNetworkAccessKey( ESocialNetwork eSocialNetwork, DWORD dwUserIndex, bool bUsingKinect, DWORD dwUserTrackingIndex, bool bShowNetworkSignin );
*/

public:
	// Retrieve singleton instance.
	static CSocialManager* Instance();

	// To be called once during game init.
	void	Initialise();

	// Tick the social manager. Only does anything in async mode, polls for results of async actions.
	void	Tick();

	// May need to be called if something changes (i.e. player signs in to live ).
	bool	RefreshPostingCapability();

	// Returns true if any social newtork posting is allowed by us, false if not (if false, game must not display any social network UI).
	bool	IsTitleAllowedToPostAnything();

	// Returns true if we are allowed to post images to social networks.
	bool	IsTitleAllowedToPostImages();

	// Returns true if we are allowed to post links to social networks.
	bool	IsTitleAllowedToPostLinks();

	// Returns false if any of the live signed in users have disabled XPRIVILEGE_SOCIAL_NETWORK_SHARING
	bool	AreAllUsersAllowedToPostImages();

	// Post a test link to social network.
	bool	PostLinkToSocialNetwork( ESocialNetwork eSocialNetwork, DWORD dwUserIndex, bool bUsingKinect );

	// Post a test image to social network.
	bool	PostImageToSocialNetwork( ESocialNetwork eSocialNetwork, DWORD dwUserIndex, bool bUsingKinect );

	void	SetSocialPostText(LPCWSTR Title, LPCWSTR Caption, LPCWSTR Desc);

	// WESTY : Not sure if we even need to get social access key!
/*
	// We do not currently know what this is used for. We may not even need it?
	bool	ObtainSocialNetworkAccessKey( ESocialNetwork eSocialNetwork, DWORD dwUserIndex, bool bUsingKinect );
	*/

private:
	WCHAR m_wchTitleA[MAX_SOCIALPOST_CAPTION+1];
	WCHAR m_wchCaptionA[MAX_SOCIALPOST_CAPTION+1];
	WCHAR m_wchDescA[MAX_SOCIALPOST_DESC+1];

};

#endif //_SOCIAL_MANAGER_H
