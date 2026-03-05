// =================================================================================================================================
// This sample demonstrates how to use the auto-hooking functionality of HeapInspector. On PlayStation 3, this uses link-time
// malloc/realloc/free overloading. On PC it uses the mhook API, which will hook into the very low-level HeapAlloc, HeapRealloc
// and HeapFree functions. This will cause any allocations in your application to be caught. 
//
// NOTE: the platform specific hooking calls can be found in the Main<platform>.cpp.
// =================================================================================================================================

#include <stdlib.h>

void Wait(int a_MilliSeconds);

void RunHeapInspectorServer()
{
	while (1)
	{
		void* mem1;
		void* memB;
		
		mem1 = malloc(16);
		memB = malloc(1024);

		Wait(100);

		free(mem1);
		free(memB);

		Wait(100);
	}
}
