/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "util/GzipUtils.h"
#include "Core/Utility/utf8proc.h"
// #include "platform/AppPlatform.h"

namespace GzipUtil
{

	bool decompress(const std::string& compressedBytes, std::string& uncompressedBytes) {
		// Nothin' to do
		if (compressedBytes.size() == 0) {
			uncompressedBytes = compressedBytes;
			return true;
		}

		// Set up the uncompressed byte buffer
		unsigned uncompLength = compressedBytes.size();
		unsigned uncompLengthHalf = uncompLength / 2;
		uncompressedBytes.resize(uncompLength);

		// I am now writing C code in C++
		z_stream strm;
		strm.next_in = (Bytef *)compressedBytes.c_str();
		strm.avail_in = compressedBytes.size();
		strm.total_out = 0;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;

		// Arbitrary 16 bits added here for the gzip header and trailer around the compressed data
		if (inflateInit2(&strm, (16 + MAX_WBITS)) != Z_OK) {
			return false;
		}

		int progress = Z_OK;

		// Will exit loop on Z_STREAM_END or any error
		while (progress == Z_OK) {
			// Increase buffer size if needed
			if (strm.total_out >= uncompLength) {
				// Increase size of output buffer
				uncompLength += uncompLengthHalf;
				uncompressedBytes.resize(uncompLength);
			}

			strm.next_out = (Bytef *)(uncompressedBytes.data() + strm.total_out);
			strm.avail_out = uncompLength - strm.total_out;

			// Inflate another chunk, if Z_OK, then continue
			progress = inflate(&strm, Z_SYNC_FLUSH);
		}

		// Trim away the buffer bytes at the end
		uncompressedBytes.resize(strm.total_out);

		// An error occurred when inflating
		if (inflateEnd(&strm) != Z_OK) {
			return false;
		}

		return true;
	}

	bool compress(const std::string& uncompressedBytes, std::string& compressedBytes) {
		// Nothin' to do
		if (uncompressedBytes.size() == 0) {
			compressedBytes = uncompressedBytes;
			return true;
		}

		// Set up the compressed byte buffer (start quite small, compression should be smaller than the OG)
		unsigned compLength = static_cast<unsigned>(uncompressedBytes.size() * 0.1f);
		unsigned compAddition = sizeof(unsigned);
		compLength -= compLength % compAddition; // Make the length be a nice factor of the addition
		compressedBytes.resize(compLength);

		// I am now writing C code in C++
		z_stream strm;
		strm.next_in = (Bytef *)uncompressedBytes.c_str();
		strm.avail_in = uncompressedBytes.size();
		strm.total_out = 0;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;

		// Arbitrary 16 bits added here for the gzip header and trailer around the compressed data
		if (deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, (16 + MAX_WBITS), 8, Z_DEFAULT_STRATEGY) != Z_OK) {
			return false;
		}

		int progress = Z_OK;

		// Will exit loop on Z_STREAM_END or any error (as long as there is stuff to be read)
		while (progress == Z_OK) {
			// Increase buffer size if needed
			if (strm.total_out >= compLength) {
				// Increase size of output buffer
				compLength += compAddition;
				compressedBytes.resize(compLength);
			}

			strm.next_out = (Bytef *)(compressedBytes.data() + strm.total_out);
			strm.avail_out = compLength - strm.total_out;

			// Deflate another chunk, if Z_OK, then continue
			progress = deflate(&strm, Z_FINISH);
		}

		// An error occurred when deflating
		if (deflateEnd(&strm) != Z_OK) {
			return false;
		}

		return true;
	}

	bool decompressNBTFromAssets(const std::string& filename, std::string& data) {
		// Input file data
		std::string fileData;
		fileData.reserve(512);

		// Read the gzip file into a string to prepare for decompression
		// TODO: rherlitz fix
// 		std::string readData = AppPlatform::singleton().readAssetFile(filename);
		std::string readData = "";
		if (readData.empty()) {
			return false;
		}

		// Uncompress the file data  
		return decompress(readData, data);
	}

	bool compressNBT(const std::string& filename, const std::string& data) {
		// Output file data
		std::string fileData;

		// Compress the file data  
		if (compress(data, fileData)) {
			// TODO: Bug 39531: This doesn't work on Android as opening files is different.
			// Filing a bug to fix as this is only needed for structure saving using
			// structure blocks, which we aren't shipping for 0.17.

			// Prepare an output file
			FILE* file = fopen(filename.c_str(), "wb");
			if (!file) {
				return false;
			}

			fwrite(fileData.data(), sizeof(Bytef), fileData.size(), file);
			fclose(file);

			return true;
		}

		return false;
	}

}	// GzipUtil
