/********************************************************
*                                                       *
* Copyright (C) Microsoft. All rights reserved.         *
*                                                       *
********************************************************/

// Sentient Client File API
//
// Include this to get access to all File-related Sentient features.

#pragma once

#include "SenClientTypes.h"


namespace Sentient
{
	//=======================//
	//                       //
	//    File Data Types    //
	//                       //
	//=======================//

	struct SenRawDataTransferInfo;

	/// @brief           A unique file identifier.
	///
	/// @details         A SenFileID represents a unique revision of a file in the file DB.
	///                  These should only be obtained from the API and never specified manually,
    ///                  with the exception of the SenFileID_Invalid enumeration below.
    enum SenFileID : INT32
    {
		/// A non-existent file.  This is expected when a file is not found, for instance.
        SenFileID_Invalid = 0x00000000,  
    };

	/// @brief           Stores a digest of a file.
	///
	/// @details         This is a 128-bit digest, checksum, CRC, etc. (currently an MD5) of a file.
    ///                  We typically use this to determine whether a file's contents are different or corrupt.
	///
    struct SenFileDigest
    {
        UINT8 value[16];  ///< The raw bytes of the digest.
    };

	/// @brief           Basic file transfer information.
	/// 
	/// @details         Resource-specific data structures use this, or a SenPackageFileTransferInfo,
	///                  to indicate how one can transfer a file to the console.
	///                  The internal file download routine takes this structure directly.
	///
    struct SenFileTransferInfo
    {
        SenFileID      fileID;    ///< Used for locating the file data on the server.
        SenSysFileSize fileSize;  ///< The expected size of the file data.  Don't read this directly--use the GetFileSize() method;
        SenFileDigest  digest;    ///< The expected file digest of the file data.

		/// @brief           Test to see if this contains a real file.
		///
		/// @details         Returns true if this structure appears to refer to an actual file,
		///                  but does not tell you if the file actually exists on the server.
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


	//======================//
	//                      //
	//    File Functions    //
	//                      //
	//======================//

	// Handy utility functions to handle digests:

	/// @brief           Calculate a SenFileDigest from a buffer.
	///
	/// @param[in]       dataSize
	///                  Number of bytes in data.
	///
	/// @param[in]       data
	///                  The actual raw data of the file.
	///
	/// @return          A SenFileDigest calculated from the passed data.
	///
	/// @details         Calculate a SenFileDigest from a buffer.  Currently, this is an MD5.
	///
	/// @related         SenFileDownload()
	/// @related         SenFileDigestCreate()
	///
	SenFileDigest SenFileDigestCreate( size_t dataSize, const BYTE *data );

	/// @brief           Compare two SenFileDigests for equality.
	///
	/// @param[in]       value0
	///                  This is the left value for the comparison.
	///
	/// @param[in]       value1
	///                  This is the right value for the comparison.
	///
	/// @return          Returns a bool: true if value0==value1, false if not.
	///
	bool operator ==( const SenFileDigest &value0, const SenFileDigest &value1 );

	/// @brief           Compare two SenFileDigests for inequality.
	///
	/// @param[in]       value0
	///                  This is the left value for the comparison.
	///
	/// @param[in]       value1
	///                  This is the right value for the comparison.
	///
	/// @return          Returns a bool: true if value0!=value1, false if not.
	///
	bool operator !=( const SenFileDigest &value0, const SenFileDigest &value1 );

} // namespace Sentient
