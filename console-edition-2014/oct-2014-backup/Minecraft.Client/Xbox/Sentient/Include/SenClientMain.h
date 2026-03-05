/********************************************************
*                                                       *
* Copyright (C) Microsoft. All rights reserved.         *
*                                                       *
********************************************************/

// Sentient Client Main API
//
// Include this to get access to all core Sentient features.

#pragma once

#include "SenClientTypes.h"


namespace Sentient
{
	//=======================//
	//                       //
	//    Main Data Types    //
	//                       //
	//=======================//

	// None at the moment.


	//======================//
	//                      //
	//    Main Functions    //
	//                      //
	//======================//

	/// @brief           Initialize and start Sentient.
	///
	/// @param[in]       titleID
	///                  Tells Sentient what our titleID is.
	///
	/// @return          Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.  Specific values include:
	///                  E_FAIL (and other E_* codes): Failed to initialize.
	///                  S_OK: Initialized successfully.
	///
	/// @details         Call this on startup to set up networking system and initialize internal buffers.
	///
	HRESULT SentientInitialize( 
		SenSysTitleID titleID );

	/// @brief           Update Sentient's internal state.
	///
	/// @return          Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine overall success.
	///                  The return code contains information about the status of the connection to sentient, including:
	///                  S_OK: we have a working connection to the Sentient server.
	///                  SENTIENT_S_NOT_SIGNED_IN_TO_LIVE: no live enabled user user is currently signed in to live.
	///                  SENTIENT_S_INITIALIZING_CONNECTION: a user has signed in, and we have started first connection attempt, but we have neither succeeded nor failed yet.
	///                  SENTIENT_S_SERVER_CONNECTION_FAILED: a connection attempt has failed, or that an existing connection was lost.
	///
	/// @details         Call this every frame to handle network message pumping and to trigger asynchronous callbacks on completed ( or failed ) tasks.
	///
	HRESULT SentientUpdate();

	/// @brief           Stop and uninitialize Sentient.
	///
	/// @return          Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.  Specific values include:
	///                  E_FAIL (and other E_* codes): Failed to shut down.
	///                  S_OK: Shut down successfully.
	///                  Note that one should consider the library shut down even if an error code is returned.
	///
	/// @details         Call this on app/game shutdown. (not necessary on X360)
	///
	HRESULT SentientShutdown();

	/// @brief           Cancel an asynchronous task.
	///
	/// @param[in]       task
	///                  The task to cancel.
	///
	/// @return          Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.  Specific values include:
	///                  E_FAIL (and other E_* codes): Failed to cancel task.
	///                  S_OK: Task cancelled successfully.
	///
	/// @detail          Call this to immediately cancel any task that exposes a SenHandle.
	///                  The completion callback will be invoked on a successful cancel.
	///
	HRESULT SentientCancel( SenHandle task );

} // namespace Sentient
