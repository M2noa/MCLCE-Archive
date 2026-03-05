#pragma once


typedef enum _XBACKGROUND_DOWNLOAD_MODE
{
	XBACKGROUND_DOWNLOAD_MODE_ALWAYS_ALLOW = 1,
	XBACKGROUND_DOWNLOAD_MODE_AUTO = 2
} XBACKGROUND_DOWNLOAD_MODE, *PXBACKGROUND_DOWNLOAD_MODE;

DWORD XBackgroundDownloadSetMode(
		 XBACKGROUND_DOWNLOAD_MODE Mode
);

#define XONLINE_S_LOGON_CONNECTION_ESTABLISHED 0
#define XPARTY_E_NOT_IN_PARTY 0
#define XPARTY_MAX_USERS 4
typedef struct _XPARTY_USER_INFO {
	PlayerUID Xuid;
	CHAR GamerTag[XUSER_NAME_SIZE];
	DWORD dwUserIndex;
 //   XONLINE_NAT_TYPE NatType;
	DWORD dwTitleId;
	DWORD dwFlags;
//    XSESSION_INFO SessionInfo;
//    XPARTY_CUSTOM_DATA CustomData;
} XPARTY_USER_INFO;

typedef struct _XPARTY_USER_LIST {
	DWORD dwUserCount;
	XPARTY_USER_INFO Users[XPARTY_MAX_USERS];
} XPARTY_USER_LIST;


DWORD XShowPartyUI(DWORD dwUserIndex);
DWORD XShowFriendsUI(DWORD dwUserIndex);
HRESULT XPartyGetUserList(XPARTY_USER_LIST *pUserList);

DWORD XContentGetThumbnail(DWORD dwUserIndex, const XCONTENT_DATA *pContentData,  PBYTE pbThumbnail,  PDWORD pcbThumbnail,  PXOVERLAPPED *pOverlapped);

void XShowAchievementsUI(int i);

DWORD XUserAreUsersFriends(
		 DWORD dwUserIndex,
		 PPlayerUID pXuids,
		 DWORD dwXuidCount,
		 PBOOL pfResult,
		 void *pOverlapped);

class XSOCIAL_IMAGEPOSTPARAMS
{
};

class XSOCIAL_LINKPOSTPARAMS
{
};

typedef struct _XSESSION_VIEW_PROPERTIES
{
	DWORD dwViewId;
	DWORD dwNumProperties;
	XUSER_PROPERTY *pProperties;
} XSESSION_VIEW_PROPERTIES;

#define XUSER_STATS_ATTRS_IN_SPEC 1

typedef struct _XUSER_STATS_SPEC
{
	DWORD dwViewId;
	DWORD dwNumColumnIds;
	WORD rgwColumnIds[XUSER_STATS_ATTRS_IN_SPEC];
} XUSER_STATS_SPEC, *PXUSER_STATS_SPEC;

typedef struct _XUSER_STATS_COLUMN {
	WORD wColumnId;
	XUSER_DATA Value;
} XUSER_STATS_COLUMN, *PXUSER_STATS_COLUMN;


typedef struct _XUSER_STATS_ROW {
	PlayerUID xuid;
	DWORD dwRank;
	LONGLONG i64Rating;
	CHAR szGamertag[XUSER_NAME_SIZE];
	DWORD dwNumColumns;
	PXUSER_STATS_COLUMN pColumns;
} XUSER_STATS_ROW, *PXUSER_STATS_ROW;


typedef struct _XUSER_STATS_VIEW {
	DWORD dwViewId;
	DWORD dwTotalViewRows;
	DWORD dwNumRows;
	PXUSER_STATS_ROW pRows;
} XUSER_STATS_VIEW, *PXUSER_STATS_VIEW;


typedef struct _XUSER_STATS_READ_RESULTS {
	DWORD dwNumViews;
	PXUSER_STATS_VIEW pViews;
} XUSER_STATS_READ_RESULTS, *PXUSER_STATS_READ_RESULTS;


