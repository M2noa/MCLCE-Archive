/************************************************************************
*                                                                       *
*   xcompress.h -- This module defines the Xbox Compression APIs        *
*                                                                       *
*   Copyright (c) Microsoft Corp. All rights reserved.                  *
*                                                                       *
************************************************************************/
#pragma once

#ifndef _XCOMPRESS_H_
#define _XCOMPRESS_H_

#include <sal.h>

#ifndef XBOXAPI
#define XBOXAPI
#endif

#ifdef __cplusplus
#define XCTDEXTERN extern "C"
#else
#define XCTDEXTERN extern
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#pragma warning(push)
#pragma warning(disable: 4200) // zero-sized array

typedef enum _XMEMCODEC_TYPE
{
    XMEMCODEC_DEFAULT =                     0,
    XMEMCODEC_LZX =                         1
} XMEMCODEC_TYPE;

/*
 * Data compression flags
 */

#define XMEMCOMPRESS_STREAM                 0x00000001
 
/*
 * Data compression functions
 */

typedef VOID*                               XMEMCOMPRESSION_CONTEXT;

XBOXAPI
HRESULT 
WINAPI 
XMemCreateCompressionContext(
    __in        XMEMCODEC_TYPE              CodecType,
    __in_opt    CONST VOID*                 pCodecParams,
    __in        DWORD                       Flags,
    __deref_out XMEMCOMPRESSION_CONTEXT*    pContext
    );

XBOXAPI
XMEMCOMPRESSION_CONTEXT
WINAPI 
XMemInitializeCompressionContext(
    __in        XMEMCODEC_TYPE              CodecType,
    __in_opt    CONST VOID*                 pCodecParams,
    __in        DWORD                       Flags,
    __out_bcount(ContextSize) VOID*         pContextData,
    __in        SIZE_T                      ContextSize
    );

XBOXAPI
VOID
WINAPI 
XMemDestroyCompressionContext(
    __in        XMEMCOMPRESSION_CONTEXT     Context
    );

XBOXAPI
SIZE_T
WINAPI 
XMemGetCompressionContextSize(
    __in        XMEMCODEC_TYPE              CodecType,
    __in_opt    CONST VOID*                 pCodecParams,
    __in        DWORD                       Flags
    );

XBOXAPI
HRESULT 
WINAPI 
XMemResetCompressionContext(
    __in    XMEMCOMPRESSION_CONTEXT         Context);

XBOXAPI
HRESULT 
WINAPI 
XMemCompress(
    __in    XMEMCOMPRESSION_CONTEXT         Context,
    __out_bcount_opt( *pDestSize ) VOID*    pDestination,
    __inout SIZE_T*                         pDestSize,
    __in_bcount( SrcSize ) CONST VOID*      pSource,
    __in    SIZE_T                          SrcSize
    );

XBOXAPI
HRESULT 
WINAPI 
XMemCompressStream(
    __in    XMEMCOMPRESSION_CONTEXT         Context,
    __out_bcount_opt( *pDestSize ) VOID*    pDestination,
    __inout SIZE_T*                         pDestSize,
    __in_bcount( *pSrcSize ) CONST VOID*    pSource,
    __inout SIZE_T*                         pSrcSize
    );

/*
 * Data decompression functions
 */

typedef VOID*                               XMEMDECOMPRESSION_CONTEXT;

XBOXAPI
HRESULT 
WINAPI 
XMemCreateDecompressionContext(
    __in        XMEMCODEC_TYPE              CodecType,
    __in_opt    CONST VOID*                 pCodecParams,
    __in        DWORD                       Flags,
    __deref_out XMEMDECOMPRESSION_CONTEXT*  pContext
    );

XBOXAPI
XMEMDECOMPRESSION_CONTEXT
WINAPI 
XMemInitializeDecompressionContext(
    __in        XMEMCODEC_TYPE              CodecType,
    __in_opt    CONST VOID*                 pCodecParams,
    __in        DWORD                       Flags,
    __out_bcount( ContextSize ) VOID*       pContextData,
    __in        SIZE_T                      ContextSize
    );

XBOXAPI
VOID
WINAPI 
XMemDestroyDecompressionContext(
    __in    XMEMDECOMPRESSION_CONTEXT       Context
    );

XBOXAPI
SIZE_T
WINAPI 
XMemGetDecompressionContextSize(
    __in        XMEMCODEC_TYPE              CodecType,
    __in_opt    CONST VOID*                 pCodecParams,
    __in        DWORD                       Flags
    );

XBOXAPI
HRESULT 
WINAPI 
XMemResetDecompressionContext(
    __in    XMEMDECOMPRESSION_CONTEXT       Context);

XBOXAPI
HRESULT 
WINAPI 
XMemDecompress(
    __in    XMEMDECOMPRESSION_CONTEXT       Context,
    __out_bcount( *pDestSize ) VOID*        pDestination,
    __inout SIZE_T*                         pDestSize,
    __in_bcount( SrcSize) CONST VOID*       pSource,
    __in    SIZE_T                          SrcSize
    );

XBOXAPI
HRESULT 
WINAPI 
XMemDecompressStream(
    __in    XMEMDECOMPRESSION_CONTEXT       Context,
    __out_bcount( *pDestSize ) VOID*        pDestination,
    __inout SIZE_T*                         pDestSize,
    __in_bcount( *pSrcSize ) CONST VOID*    pSource,
    __inout SIZE_T*                         pSrcSize
    );

/*
 * LZX codec for lossless compression
 */

typedef struct _XMEMCODEC_PARAMETERS_LZX
{
    DWORD Flags;
    DWORD WindowSize;
    DWORD CompressionPartitionSize;
} XMEMCODEC_PARAMETERS_LZX;

#define XCOMPRESS_LZX_BLOCK_SIZE            (32 * 1024)
#define XCOMPRESS_LZX_BLOCK_GROWTH_SIZE_MAX 6155

/*
 * Error codes
 */

#define XMCDERR_MOREDATA                    _HRESULT_TYPEDEF_(0x81DE2001)

/*
 * Compression file headers
 */

#define XCOMPRESS_FILE_IDENTIFIER_LZXTDECODE        0x0FF512ED
#define XCOMPRESS_FILE_IDENTIFIER_LZXNATIVE         0x0FF512EE

#define XCOMPRESS_SET_FILE_VERSION(Major, Minor)    ((((Major) & 0xFF) << 8) | ((Minor) & 0xFF))
#define XCOMPRESS_GET_FILE_VERSION_MAJOR(Version)   (((Version) >> 8) & 0xFF)
#define XCOMPRESS_GET_FILE_VERSION_MINOR(Version)   ((Version) & 0xFF)

#define XCOMPRESS_LZXNATIVE_VERSION_MAJOR           1
#define XCOMPRESS_LZXNATIVE_VERSION_MINOR           3

typedef struct _XCOMPRESS_FILE_HEADER
{
    DWORD                                   Identifier;
    WORD                                    Version;
    WORD                                    Reserved;
} XCOMPRESS_FILE_HEADER;

typedef struct _XCOMPRESS_FILE_HEADER_LZXNATIVE
{
    XCOMPRESS_FILE_HEADER                   Common;
    DWORD                                   ContextFlags;
    XMEMCODEC_PARAMETERS_LZX                CodecParams;
    DWORD                                   UncompressedSizeHigh;
    DWORD                                   UncompressedSizeLow;
    DWORD                                   CompressedSizeHigh;
    DWORD                                   CompressedSizeLow;
    DWORD                                   UncompressedBlockSize;
    DWORD                                   CompressedBlockSizeMax;
} XCOMPRESS_FILE_HEADER_LZXNATIVE;

typedef struct _XCOMPRESS_BLOCK_HEADER_LZXNATIVE
{
    DWORD                                   CompressedBlockSize;
    BYTE                                    pCompressedData[0];
} XCOMPRESS_BLOCK_HEADER_LZXNATIVE;

#pragma warning(pop)

#ifdef __cplusplus
}
#endif

#endif /* _XCOMPRESS_H_ */
