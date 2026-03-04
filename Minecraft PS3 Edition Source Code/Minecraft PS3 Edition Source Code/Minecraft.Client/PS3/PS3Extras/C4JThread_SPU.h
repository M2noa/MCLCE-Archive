

#pragma once
#undef __in
#undef __out

// standard C libraries
#include <cstdio>						// printf
#include <cstdlib>						// abort
#include <cstring>						// strlen
#include <sys/spu_thread_group.h>		// SYS_SPU_THREAD_GROUP_TYPE_EXCLUSIVE_NON_CONTEXT
#include <sys/process.h>				// SYS_PROCESS_PARAM
#include <cell/spurs.h>

#ifndef _CONTENTPACKAGE
#include <spu_printf.h>
#endif

class C4JThread_SPU
{

	static cell::Spurs::Spurs2* ms_spurs2Object;


public:

	static void initSPURS();
	static void shutdownSPURS();
	static cell::Spurs::Spurs2* getSpurs2() { return ms_spurs2Object; }

};