/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

	xsgx_xtms.h

--*/
#include "xtms.h"

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef XSGX_MOD1

typedef
VOID
(CALLBACK * TMSCLIENT_PROG_CALLBACK)(
	__in float progress,
	__in_opt void* userCallbackData
	);


XBOXAPI
HRESULT
WINAPI
XSGX_XTmsPutUserFile(
	__in      HTMSCLIENT          tmsClient,
	__in      XUID                xuid, 
	__in_z    LPCSTR              filePath, 
	__in_z    LPCSTR              fileType, 
	__in_z    LPCSTR              displayName,
	__in_z    LPCSTR              ugcContentType,
	__in_bcount(fileSize) CONST CHAR* fileBuffer,
	__in      DWORD               fileSize,
	__in_opt  DWORD               bufferSize,
	__in_z_opt LPCSTR             etag,
	__in      ETAGFLAG            etagFlag,
	__in      TMSCLIENT_CALLBACK  clientCallback,
	__in_opt  PVOID               userCallbackData,
	__in      TMSCLIENT_PROG_CALLBACK  clientProgCallback,
	__in_opt  PVOID               progCallbackData
	);

void XSGX_XTmsCancelPutUserFile(__in      HTMSCLIENT          tmsClient);

#endif

#if defined(__cplusplus)
}
#endif
