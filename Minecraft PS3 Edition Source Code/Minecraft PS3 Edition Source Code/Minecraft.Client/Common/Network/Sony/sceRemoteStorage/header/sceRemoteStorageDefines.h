
#ifndef SCE_REMOTE_STORAGE_DEFINES_H
#define SCE_REMOTE_STORAGE_DEFINES_H

#if defined(__psp2__)
#include <stddef.h>
#include <apputil.h>
#elif __PS3__
#define SceAppUtilSaveDataDataSlot int
#endif

#include <stdint.h>

// Macros
#define SCE_REMOTE_STORAGE_MAX_FILES						16
#define SCE_REMOTE_STORAGE_DATA_NAME_MAX_LEN				64
#define SCE_REMOTE_STORAGE_CLIENT_ID_MAX_LEN				64
#define SCE_REMOTE_STORAGE_PLATFORM_NAME_MAX_LEN			16
#define SCE_REMOTE_STORAGE_MD5_STRING_LENGTH				33
#define SCE_REMOTE_STORAGE_RFC2822_LENGTH					32
#define SCE_REMOTE_STORAGE_DATA_DESCRIPTION_MAX_LEN			256
#define SCE_REMOTE_STORAGE_DATA_LOCATION_MAX_LEN			256
#define SCE_REMOTE_STORAGE_PS3_SAVEDATA_SECUREFILEID_SIZE	16
#define SCE_REMOTE_STORAGE_PS3_SAVEDATA_FILENAME_SIZE		13
#define SCE_REMOTE_STORAGE_AUTH_CODE_MAX_LEN				128

// Return values
#define SCE_REMOTE_STORAGE_SUCCESS							0

// Error codes
#define SCE_REMOTE_STORAGE_ERROR_INVALID_ARGUMENT			0x80001001
#define SCE_REMOTE_STORAGE_ERROR_FAILED_TO_CREATE_THREAD	0x80001002
#define SCE_REMOTE_STORAGE_ERROR_NOT_INITIALISED			0x80001003
#define SCE_REMOTE_STORAGE_ERROR_FAILED_TO_OPEN_WEB_BROWSER	0x80001004
#define SCE_REMOTE_STORAGE_ERROR_PSN_ACCOUNT_NOT_LINKED		0x80001005
#define SCE_REMOTE_STORAGE_ERROR_COULD_NOT_CREATE_SESSION	0x80001006
#define SCE_REMOTE_STORAGE_ERROR_FAILED_TO_ALLOCATE			0x80001007
#define SCE_REMOTE_STORAGE_ERROR_SESSION_DOES_NOT_EXIST		0x80001008
#define SCE_REMOTE_STORAGE_ERROR_REQ_ID_NOT_FOUND			0x80001009
#define SCE_REMOTE_STORAGE_ERROR_MAX_NUMBER_FILES_REACHED	0x8000100A
#define SCE_REMOTE_STORAGE_ERROR_NO_MORE_SYNCS				0x8000100B
#define SCE_REMOTE_STORAGE_ERROR_ALREADY_INITIALISED		0x8000100C
#define SCE_REMOTE_STORAGE_ERROR_INVALID_UPLOADID			0x8000100D
#define SCE_REMOTE_STORAGE_ERROR_FAILED_TO_OPEN_FILE		0x8000100E
#define SCE_REMOTE_STORAGE_ERROR_CLOUD_DATA_CORRUPTED		0x8000100F
#define SCE_REMOTE_STORAGE_ERROR_INVALID_CHAR_IN_FILE_NAME	0x80001010
#define SCE_REMOTE_STORAGE_ERROR_INVALID_JSON_RESPONSE		0x80001011
#define SCE_REMOTE_STORAGE_ERROR_REQUEST_ABORTED			0x80001012
#define SCE_REMOTE_STORAGE_ERROR_SERVER_ERROR				0x80002000 // Server errors can be between 0x80002064 to 0x800022BB both included


typedef enum SceRemoteStorageDataVisibility
{
	PRIVATE = 0,						// Only data owner can read and write data
	PUBLIC_READ_ONLY,					// Everyone can read this data. Owner can write to it
	PUBLIC_READ_WRITE					// Everyone can read and write data
} SceRemoteStorageDataVisibility;

typedef enum SceRemoteStorageEvent
{
	USER_ACCOUNT_LINKED = 0,			// User's account has been linked with PSN
	PSN_SIGN_IN_REQUIRED,				// User's PSN sign-in through web browser is required
	WEB_BROWSER_RESULT,					// Result of sceRemoteStorageOpenWebBrowser(). Please check retCode
	GET_DATA_RESULT,					// Result of sceRemoteStorageGetData(). Please check retCode
	GET_DATA_PROGRESS,					// Progress of sceRemoteStorageGetData() completion as a percentage. Please check retCode
	SET_DATA_RESULT,					// Result of sceRemoteStorageSetData(). Please check retCode
	SET_DATA_PROGRESS,					// Progress of sceRemoteStorageSetData() completion as a percentage. Please check retCode
	GET_STATUS_RESULT,					// Result of sceRemoteStorageGetStatus(). Please check retCode
	ERROR_OCCURRED						// A generic error has occurred. Please check retCode
} SceRemoteStorageEvent;

typedef enum SceRemoteStorageEnvironment
{
	DEVELOPMENT = 0,
	PRODUCTION
}SceRemoteStorageEnvironment;

typedef void (*sceRemoteStorageCallback)(const SceRemoteStorageEvent event, int32_t retCode, void * userData);

typedef struct SceRemoteStorageInitParamsThread
{
	int32_t threadAffinity;				// Thread affinity
	int32_t threadPriority;				// Priority that the thread runs out
} SceRemoteStorageInitParamsThread;

typedef struct SceRemoteStorageInitParamsPool
{
	void * memPoolBuffer;				// Memory pool used by sceRemoteStorage library
	size_t memPoolSize;					// Size of memPoolBuffer
} SceRemoteStorageInitParamsPool;

typedef struct SceRemoteStorageInitTimeout
{
	uint32_t resolveMs;					//Timeout for DNS resolution in milliseconds. Defaults to 30 seconds
	uint32_t connectMs;					//Timeout for first connection between client and server. Defaults to 30 seconds
	uint32_t sendMs;					//Timeout to send request to server. Defaults to 120 seconds
	uint32_t receiveMs;					//Timeout to receive information from server. Defaults to 120 seconds
	
	SceRemoteStorageInitTimeout() : resolveMs(30 * 1000), connectMs(30 * 1000), sendMs(120 * 1000), receiveMs(120 * 1000) {}
}SceRemoteStorageInitTimeout;

typedef struct SceRemoteStorageInitParams
{
	sceRemoteStorageCallback callback;										// Event callback
	void * userData;														// Application defined data for callback
	int32_t httpContextId;													// PS4 only: Http context ID that was returned from sceHttpInit()
	int32_t userId;															// PS4 only: Current user, see SceUserServiceUserId
	void * psnTicket;														// PS3 only: The PSN ticket used to authenticate the user
	size_t psnTicketSize;													// PS3 only: The size of the PSN ticket in bytes
	char clientId[SCE_REMOTE_STORAGE_CLIENT_ID_MAX_LEN];					// This represents your application on PSN, used to sign PSN user in for your title
	SceRemoteStorageInitTimeout timeout;									// Timeout for network transactions
	SceRemoteStorageInitParamsPool pool;									// Memory pool parameters
	SceRemoteStorageInitParamsThread thread;								// Thread creation parameters
	SceRemoteStorageEnvironment environment;								// Only used on non-PlayStation platforms: PSN Environment used by the library 
} SceRemoteStorageInitParams;

typedef struct SceRemoteStorageGetDataReqParams
{
	char fileName[SCE_REMOTE_STORAGE_DATA_NAME_MAX_LEN];					// Name of file on remote storage server
	char pathLocation[SCE_REMOTE_STORAGE_DATA_LOCATION_MAX_LEN];			// File location on the HDD
	char secureFileId[SCE_REMOTE_STORAGE_PS3_SAVEDATA_SECUREFILEID_SIZE];	// PS3 only. ID used for save data encryption
	char ps3DataFilename[SCE_REMOTE_STORAGE_PS3_SAVEDATA_FILENAME_SIZE];	// PS3 only. Name of data file in save data
	uint32_t ps3FileType;													// PS3 only. Type of file, CELL_SAVEDATA_FILETYPE_XXX
	SceAppUtilSaveDataDataSlot psVitaSaveDataSlot;							// PS Vita only. Save data slot information
} SceRemoteStorageGetDataReqParams;

typedef struct SceRemoteStorageSetDataReqParams
{
	char fileName[SCE_REMOTE_STORAGE_DATA_NAME_MAX_LEN];					// Name of file on remote storage server
	char fileDescription[SCE_REMOTE_STORAGE_DATA_DESCRIPTION_MAX_LEN];		// Description of file on remote storage server
	char pathLocation[SCE_REMOTE_STORAGE_DATA_LOCATION_MAX_LEN];			// File location on the HDD
	char secureFileId[SCE_REMOTE_STORAGE_PS3_SAVEDATA_SECUREFILEID_SIZE];	// PS3 only. ID used for save data encryption
	char ps3DataFilename[SCE_REMOTE_STORAGE_PS3_SAVEDATA_FILENAME_SIZE];	// PS3 only. Name of data file in save data
	uint32_t ps3FileType;													// PS3 only. Type of file, CELL_SAVEDATA_FILETYPE_XXX
	SceRemoteStorageDataVisibility visibility;								// Visibility of data	
} SceRemoteStorageSetDataReqParams;

typedef struct SceRemoteStorageData
{
	char fileName[SCE_REMOTE_STORAGE_DATA_NAME_MAX_LEN];					// Name of file on remote storage server
	char fileDescription[SCE_REMOTE_STORAGE_DATA_DESCRIPTION_MAX_LEN];		// Description of file on remote storage server
	size_t fileSize;														// Size of file in bytes
	char md5Checksum[SCE_REMOTE_STORAGE_MD5_STRING_LENGTH];					// File MD5 checksum
	char timeStamp[SCE_REMOTE_STORAGE_RFC2822_LENGTH];						// Time that data was written on the server. Format is RFC2822
	SceRemoteStorageDataVisibility visibility;								// Visibility of data
} SceRemoteStorageData;

typedef struct SceRemoteStorageWebBrowserReqParams { } SceRemoteStorageWebBrowseReqParams;

typedef struct SceRemoteStorageStatusReqParams { } SceRemoteStorageStatusReqParams;

typedef struct SceRemoteStorageAbortReqParams 
{
	uint32_t requestId;														// The request Id to be aborted
} SceRemoteStorageAbortReqParams;

typedef struct SceRemoteStorageStatus
{
	uint32_t numFiles;														// Number of files user has on remote storage server
	SceRemoteStorageData data[SCE_REMOTE_STORAGE_MAX_FILES];				// Details about data if available. Data buffer will not be retrieved
	uint64_t remainingSyncs;												// Remaining syncs. the user has for upload/download
} SceRemoteStorageStatus;

#endif
