
#pragma once
#include <stdio.h>
#include <libsn.h>

#ifdef _CONTENT_PACKAGE
#define PS3_ASSERT(val)
#define PS3_ASSERT_CELL_ERROR(errVal)
#elif defined(_RELEASE_FOR_ART)
#define PS3_ASSERT(val)
#define PS3_ASSERT_CELL_ERROR(errVal)
#else
#define PS3_ASSERT(val) if(!(val)) { printf("----------------------\n"); printf("Assert failed!!!\n"); printf(#val); printf("\n"); printf("%s failed.\nFile %s\nline %d \n----------------------\n", __FUNCTION__, __FILE__, __LINE__); snPause(); }
#define PS3_ASSERT_CELL_ERROR(errVal) if(errVal != CELL_OK) { printf("----------------------\n    %s failed with error %d [0x%08x]\n----------------------\n", __FUNCTION__, errVal, errVal); PS3_ASSERT(0); }
#endif

#define assert PS3_ASSERT
