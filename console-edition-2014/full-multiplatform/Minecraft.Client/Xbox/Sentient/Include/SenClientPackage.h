/********************************************************
*                                                       *
* Copyright (C) Microsoft. All rights reserved.         *
*                                                       *
********************************************************/

// Sentient Client Package API
//
// Include this to get access to all Package-related Sentient features.

#pragma once

#include "SenClientTypes.h"


namespace Sentient
{
	//==========================//
	//                          //
	//    Package Data Types    //
	//                          //
	//==========================//

	/// Private.
	struct SenPackageInfo;

	/// @brief           Basic file transfer information.
	/// 
	/// @details         Resource-specific data structures use this, or a SenFileTransferInfo,
	///                  to indicate how one can transfer a file to the console.
	///                  The internal package file download routine takes this structure directly.
	///
	struct SenPackageFileTransferInfo
	{
		const SenPackageInfo *package;
		const char           *filename;
		SenSysFileSize        fileSize;

		/// @brief           Test to see if this contains a real file.
		///
		/// @details         Returns true if this structure appears to refer to an actual file,
		///                  but does not tell you if the file actually exists in a package.
		///                  Attempt to download the file to determine whether or not it exists.
		///
		bool IsValid() const;

		/// @brief           Get the actual data size.
		///
		/// @details         Returns the exact size of the data--no rounding.  Do NOT use this for
		///                  allocating destination buffers.  Use GetBufferSize() for that.
		///
		SenSysFileSize GetFileSize() const;

		/// @brief           Get the buffer size necessary to download the data.
		///
		/// @details         This will return a size which is greater than or equal to the data size.
		///                  It may be rounded up somewhat (to, e.g. a 2k boundary) for the purposes
		///                  of more efficient data transfers.  Make absolutely certain that you use
		///                  THIS size for allocating destination buffers, and NOT GetFileSize().
		///
		SenSysFileSize GetBufferSize() const;
	};


	//=========================//
	//                         //
	//    Package Functions    //
	//                         //
	//=========================//

	// None at the moment.

} // namespace Sentient
