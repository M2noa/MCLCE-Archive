/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

namespace GzipUtil
{

	// Decompresses from a compressed string into an uncompressed string
	bool decompress(const std::string& compressedBytes, std::string& uncompressedBytes);

	// Compresses from an uncompressed string into a compressed string
	bool compress(const std::string& uncompressedBytes, std::string& compressedBytes);

	// Decompresses NBT stream friendly
	bool decompressNBTFromAssets(const std::string& filename, std::string& data);

	// Compresses NBT stream friendly
	bool compressNBT(const std::string& filename, const std::string& data);

}	// GzipUtil
