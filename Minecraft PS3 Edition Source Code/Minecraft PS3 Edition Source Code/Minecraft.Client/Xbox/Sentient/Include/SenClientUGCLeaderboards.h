/********************************************************
*                                                       *
* Copyright (C) Microsoft. All rights reserved.         *
*                                                       *
********************************************************/

// Sentient Client UGC Leaderboard API
//
// Include this to get access to all UGC Leaderboard features

#pragma once

#include "SenClientUGCTypes.h"

#include "SenClientSys.h"


namespace Sentient
{
	// This system does not rely on existing XBox Leaderboards, but runs completely in parallel.

	/********************************************
	 ***** Leaderboard Creation Functions *****
	 ********************************************/

	/// @brief			Creates a dynamic Leaderboard based on a given definition.
	///
	/// @param[in]		userIndex
	///                 The index of the initiating user on the console.  
	///					Note: This is NOT a XUID.
	///
	/// @param[in]		definition
	///					Leaderboard definition that describes the behavior for the to be created Leaderboard.
	///
	/// @param[in]		metadataBlob
	///					Binary metadata blob that is associated to the created Leaderboard.
	///					Can be used to store information about the Leaderboard being created.  
	///					This information can be re-uploaded	at any time.
	///
	/// @param[in]		metadataBlobSize
	///					Used to indelicate the size of the buffer pointed to by @a metadataBlob.
	///
	/// @param[out]		out_leaderboardId
	///					The Leaderboard Id that has been created by the service.
	///
	/// @param[in]      userCallback
	///                 If this call returns a success code, 
	///					the userCallback will be called at the end of the 
	///					asynchronous process.
	///
	/// @param[in]		userCallbackData
	///                 Data to be passed to the @a userCallback on completion.
	///
	/// @return			Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.
	///					Specific values include:
	///
	/// @details		Creates a new Leaderboard based on the supplied Leaderboard Definition.
	///
	/// @related		SenCreateLeaderboard()
	HRESULT SenCreateLeaderboard(
		int userIndex,
		const SenLeaderboardDefinition &definition,
		const void *metadataBlob,
		size_t metadataBlobSize,
		SenLeaderboardId *out_leaderboardId,
		SenSysCompletedCallback callback,
		void *callbackData );

	/********************************************
	 ***** Leaderboard Set Score Functions *****
	 ********************************************/	
	
	/// @brief			Set a specific Leaderboard entry value. 
	///
	/// @param[in]		userIndex
	///                 The index of the initiating user on the console.  
	///					Note: This is NOT a XUID.
	///
	/// @param[in]		leaderboardId
	///					Leaderboard Id to set a value on.
	///
	/// @param[in]		value
	///					Value of the Leaderboard entry.
	///
	/// @param[in]      userCallback
	///                 If this call returns a success code, 
	///					the userCallback will be called at the end of the 
	///					asynchronous process.
	///
	/// @param[in]		userCallbackData
	///                 Data to be passed to the @a userCallback on completion.
	///
	/// @return			Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.
	///					Specific values include:
	///
	/// @details		The Leaderboard entry value will be set for the given Leaderboard.
	///					The actor or owner of the value will be the XUID of user
	///					logged in at the specified index.
	///
	/// @related		SenSetUserLeaderboardValue()
	__declspec(deprecated("This function is deprecated, and should not be used"))
	HRESULT SenSetUserLeaderboardValue(
		int userIndex,
		SenLeaderboardId leaderboardId, 
		SenLeaderboardEntryValue value,
		SenSysCompletedCallback callback,
		void *callbackData );

	HRESULT SenSetUserLeaderboardValue(
		int userIndex,
		SenLeaderboardId leaderboardId, 
		SenLeaderboardEntryValue value,
		LONGLONG descriptor,
		SenSysCompletedCallback callback,
		void *callbackData );

	/// @brief			Set a specific Leaderboard entry value and metadata. 
	///
	/// @param[in]		userIndex
	///                 The index of the initiating user on the console.  
	///					Note: This is NOT a XUID.
	///
	/// @param[in]		leaderboardId
	///					Leaderboard Id to set a value on.
	///
	/// @param[in]		value
	///					Value of the Leaderboard entry.
	///
	/// @param[in]		metadataBlob
	///					Binary metadata blob that is associated to the created Leaderboard Entry.
	///					Can be used to store information about the Leaderboard Entry being created.  
	///					This information can be re-uploaded	at any time.
	///
	/// @param[in]		metadataBlobSize
	///                 Used to indicate the size of the buffer pointed to by @a metadataBlob.
	///                 If the actual size of the data exceeds this, you will receive an error.
	///
	/// @param[in]      userCallback
	///                 If this call returns a success code, 
	///					the userCallback will be called at the end of the 
	///					asynchronous process.
	///
	/// @param[in]		userCallbackData
	///                 Data to be passed to the @a userCallback on completion.
	///
	/// @return			Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.
	///					Specific values include:
	///
	/// @details		The Leaderboard entry value will be set for the given Leaderboard.
	///					The actor or owner of the value will be the XUID of user
	///					logged in at the specified index.
	///
	/// @related		SenSetUserLeaderboardValue()
	__declspec(deprecated("This function is deprecated, and should not be used"))
	HRESULT SenSetUserLeaderboardValue(
		int userIndex,
		SenLeaderboardId leaderboardId, 
		SenLeaderboardEntryValue value,
		const void *metadataBlob,
		size_t metadataBlobSize,
		SenSysCompletedCallback callback,
		void *callbackData );

	HRESULT SenSetUserLeaderboardValue(
		int userIndex,
		SenLeaderboardId leaderboardId, 
		SenLeaderboardEntryValue value,
		LONGLONG descriptor,
		const void *metadataBlob,
		size_t metadataBlobSize,
		SenSysCompletedCallback callback,
		void *callbackData );

	/********************************************
	 ***** Leaderboard Retrieval Functions *****
	 ********************************************/
	
	/*For documentation: the output structure contains a pointer to a buffer that is allocated by Sentient (specifically the transport layer).  The pointer is only valid during the callback.  If title developers want to hold on to the metadata they'll need to copy the data into a buffer of their own before execution leaves the completion callback.
	It's definitely an ugly pattern which is why we strongly prefer to avoid it (and use title-allocated buffers), but I don't see a way round it in this case.*/
	
	/// @brief			Retrieves a single Leaderboard Entry. 
	///
	/// @param[in]		userIndex
	///                 The index of the initiating user on the console.  
	///					Note: This is NOT a XUID.
	///
	/// @param[in]		leaderboardId
	///					Leaderboard Id where the entry being retrieved is stored.
	///
	/// @param[in]		leaderboardFlag
	///					Flag for determining how Leaderboard specific Metadata should be retrieved.
	///
	/// @param[in]		entryFlag
	///					Flag for determining how Leaderboard Entry specific Metadata should be retrieved.
	///
	/// @param[out]		out_leaderboardEntry
	///					Leaderboard Entry being retrieved.
	///
	/// @param[in]		maxLeaderboardMetadataBlobSize
	///                 Used to indicate the size of the buffer pointed to by @a out_leaderboardMetadataBlob.
	///                 If the actual size of the data exceeds this, you will receive an error.
	///
	/// @param[out]		out_leaderboardMetadataBlob
	///					Metadata buffer associated to the Leaderboard.
	///
	/// @param[out]		out_leaderboardMetadataBlobSize
	///					Used to return the actual size of the Leaderboard Metadata being returned.
	///
	/// @param[in]      userCallback
	///                 If this call returns a success code, 
	///					the userCallback will be called at the end of the 
	///					asynchronous process.
	///
	/// @param[in]		userCallbackData
	///                 Data to be passed to the @a userCallback on completion.
	///
	/// @return			Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.
	///					Specific values include:
	///
	/// @details		Retrieves a single Leaderboard Entry.
	///					The actor or owner of the value will be the XUID of user
	///					logged in at the specified index.
	///
	/// @related		SenGetLeaderboardEntry()
	HRESULT SenGetLeaderboardEntry(
		int userIndex, 
		SenLeaderboardId leaderboardId, 
		SenLeaderboardMetadataFlag leaderboardFlag, 
		SenLeaderboardMetadataFlag entryFlag, 
		SenLeaderboardEntry *out_leaderboardEntry, 
		size_t maxLeaderboardMetadataBlobSize,			// LB
		void *out_leaderboardMetadataBlob,				// LB
		size_t *out_leaderboardMetadataBlobSize,		// LB
		SenSysCompletedCallback callback, 
		void *callbackData );
	
	/// @brief			Retrieves a single Leaderboard Entry for any given actor (XUID).
	///
	/// @param[in]		userIndex
	///                 The index of the initiating user on the console.  
	///					Note: This is NOT a XUID.
	///
	/// @param[in]		actorId
	///					Actor Id for the Leaderboard Entry being retrieved.
	///
	/// @param[in]		leaderboardId
	///					Leaderboard Id where the entry being retrieved is stored.
	///
	/// @param[in]		leaderboardFlag
	///					Flag for determining how Leaderboard specific Metadata should be retrieved.
	///
	/// @param[in]		entryFlag
	///					Flag for determining how Leaderboard Entry specific Metadata should be retrieved.
	///
	/// @param[out]		out_leaderboardEntry
	///					Leaderboard Entry being retrieved.
	///
	/// @param[in]		maxLeaderboardMetadataBlobSize
	///                 Used to indicate the size of the buffer pointed to by @a out_leaderboardMetadataBlob.
	///                 If the actual size of the data exceeds this, you will receive an error.
	///
	/// @param[out]		out_leaderboardMetadataBlob
	///					Metadata buffer associated to the Leaderboard.
	///
	/// @param[out]		out_leaderboardMetadataBlobSize
	///					Used to return the actual size of the Leaderboard Metadata being returned.
	///
	/// @param[in]      userCallback
	///                 If this call returns a success code, 
	///					the userCallback will be called at the end of the 
	///					asynchronous process.
	///
	/// @param[in]		userCallbackData
	///                 Data to be passed to the @a userCallback on completion.
	///
	/// @return			Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.
	///					Specific values include:
	///
	/// @details		Retrieves a single Leaderboard Entry for a given actor.
	///
	/// @related		SenGetLeaderboardEntry()
	HRESULT SenGetLeaderboardEntry(
		int userIndex, 
		SenLeaderboardActorId actorId,
		SenLeaderboardId leaderboardId, 
		SenLeaderboardMetadataFlag leaderboardFlag, 
		SenLeaderboardMetadataFlag entryFlag, 
		SenLeaderboardEntry *out_leaderboardEntry, 
		size_t maxLeaderboardMetadataBlobSize,			// LB
		void *out_leaderboardMetadataBlob,				// LB
		size_t *out_leaderboardMetadataBlobSize,		// LB
		SenSysCompletedCallback callback, 
		void *callbackData );


	/// @brief			Retrieve a specific Leaderboard that is ranked by Xbox Live Friends. 
	///
	/// @param[in]		userIndex
	///                 The index of the initiating user on the console.  
	///					Note: This is NOT a XUID.
	///
	/// @param[in]		leaderboardId
	///					Leaderboard Id where the entry being retrieved is stored.
	///
	/// @param[in]		leaderboardFlag
	///					Flag for determining how Leaderboard specific Metadata should be retrieved.
	///
	/// @param[in]		entryFlag
	///					Flag for determining how Leaderboard Entry specific Metadata should be retrieved.
	///
	/// @param[in]		maxLeaderboardEntries
	///					Used to indicate the number of Leaderboard Entries to be returned by @a out_leaderboardEntries.
	///					If the actual number of items exceeds this, you will receive an error.
	///
	/// @param[out]		out_leaderboardEntries
	///					Collection of Leaderboard Entries being retrieved.
	///
	/// @param[out]		out_leaderboardEntriesSize
	///					Actual size of the returned Leaderboard Entry collection.
	///
	/// @param[in]		maxLeaderboardMetadataBlobSize
	///                 Used to indicate the size of the buffer pointed to by @a out_leaderboardMetadataBlob.
	///                 If the actual size of the data exceeds this, you will receive an error.
	///
	/// @param[out]		out_leaderboardMetadataBlob
	///					Metadata buffer associated to the Leaderboard.
	///
	/// @param[out]		out_leaderboardMetadataBlobSize
	///					Used to return the actual size of the Leaderboard Metadata being returned.
	///
	/// @param[in]      userCallback
	///                 If this call returns a success code, 
	///					the userCallback will be called at the end of the 
	///					asynchronous process.
	///
	/// @param[in]		userCallbackData
	///                 Data to be passed to the @a userCallback on completion.
	///
	/// @return			Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.
	///					Specific values include:
	///
	/// @details		Returns a ranked Leaderboard.
	HRESULT SenGetLeaderboardFriends(
		int userIndex,
		SenLeaderboardId leaderboardId,
		SenLeaderboardMetadataFlag leaderboardFlag, 
		SenLeaderboardMetadataFlag entryFlag,
		size_t maxLeaderboardEntries,
		SenLeaderboardEntry *out_leaderboardEntries,
		size_t *out_leaderboardEntriesSize,
		size_t maxLeaderboardMetadataBlobSize,			// LB
		void *out_leaderboardMetadataBlob,				// LB
		size_t *out_leaderboardMetadataBlobSize,		// LB
		SenSysCompletedCallback callback, 
		void *callbackData );

	/// @brief			Retrieve a specific Leaderboard that is ranked by the top Leaderboard Entries.
	///
	/// @param[in]		userIndex
	///                 The index of the initiating user on the console.  
	///					Note: This is NOT a XUID.
	///
	/// @param[in]		leaderboardId
	///					Leaderboard Id where the entry being retrieved is stored.
	///
	/// @param[in]		maxLeaderboardEntries
	///					Used to indicate the number of Leaderboard Entries to be returned by @a out_leaderboardEntries.
	///					If the actual number of items exceeds this, you will receive an error.
	///
	/// @param[out]		out_leaderboardEntries
	///					Collection of Leaderboard Entries being retrieved.
	///
	/// @param[out]		out_leaderboardEntriesSize
	///					Actual size of the returned Leaderboard Entry collection.
	///
	/// @param[in]      userCallback
	///                 If this call returns a success code, 
	///					the userCallback will be called at the end of the 
	///					asynchronous process.
	///
	/// @param[in]		userCallbackData
	///                 Data to be passed to the @a userCallback on completion.
	///
	/// @return			Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.
	///					Specific values include:
	///
	/// @details		Returns a ranked Leaderboard of all the top Leaderboard Entries.
	HRESULT SenGetLeaderboardTop(
		int userIndex,
		SenLeaderboardId leaderboardId,
		size_t maxLeaderboardEntries,
		SenLeaderboardEntry *out_leaderboardEntries,
		size_t *out_leaderboardEntriesSize,
		SenSysCompletedCallback callback, 
		void *callbackData );

} // namespace Sentient
