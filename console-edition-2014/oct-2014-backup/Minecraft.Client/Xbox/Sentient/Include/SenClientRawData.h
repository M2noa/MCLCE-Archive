/********************************************************
*                                                       *
* Copyright (C) Microsoft. All rights reserved.         *
*                                                       *
********************************************************/

// Sentient Client RawData API
//
// Include this to get access to all RawData-related Sentient features.

#pragma once

#include "SenClientFile.h"
#include "SenClientPackage.h"


namespace Sentient
{
	//==========================//
	//                          //
	//    RawData Data Types    //
	//                          //
	//==========================//

	/// @brief           Raw data transfer information type.
	///
	/// @details         Used to denote what kind of raw data is being described in a SenRawDataTransferInfo.
	///
	enum SenRawDataType
	{
		/// A SenRawDataTransferInfo with this type contains nothing.
		SenRawDataType_Invalid,

		/// A SenRawDataTransferInfo with this type contains a SenFileTransferInfo.
		SenRawDataType_File,

		/// A SenRawDataTransferInfo with this type contains a SenPackageFileTransferInfo.
		SenRawDataType_Package,
	};

	/// @brief           Basic raw data transfer information.
	///
	/// @details         This structure contains enough information to retrieve any data.
	///                  Note: The choice of package vs. file is made internally.
	///                  Developers may use these struct contents for debugging purposes, but
	///                  should never rely on anything below the level of this struct and its
	///                  methods in their own code.  All manipulation/exploration should be API-based.
	///
	struct SenRawDataTransferInfo
	{
		SenRawDataType type;                    ///< Indicates which part of the union below to use.
		union
		{
			SenFileTransferInfo        file;    ///< Only valid if type == SenRawDataType_File;
			SenPackageFileTransferInfo package; ///< Only valid if type == SenRawDataType_Package;
		};

		/// @brief           Test to see if this contains a real file.
		///
		/// @details         Returns true if this structure appears to refer to an actual file,
		///                  but does not tell you if the file actually exists.
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

} // namespace Sentient
