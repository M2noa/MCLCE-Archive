
#pragma once
#include <stdio.h>
#include <libdbg.h>

#ifdef _CONTENT_PACKAGE
#define ORBIS_ASSERT(val)
#elif defined(_RELEASE_FOR_ART)
#define ORBIS_ASSERT(val)
#else
#define ORBIS_ASSERT(val) if(!(val)) {  printf("------------------------------------------ \n"); \
										printf("Func : %s \n", __FUNCTION__); \
										printf("File : %s \n", __FILE__); \
										printf("Line : %d \n",__LINE__ ); \
										printf("assert(%s) failed!!!\n", #val); \
										printf("------------------------------------------ \n"); \
										SCE_BREAK(); }
#endif

#define assert ORBIS_ASSERT
