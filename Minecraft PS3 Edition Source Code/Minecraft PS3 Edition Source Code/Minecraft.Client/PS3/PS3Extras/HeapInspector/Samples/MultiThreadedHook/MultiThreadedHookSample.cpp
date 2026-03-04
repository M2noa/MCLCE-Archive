// =================================================================================================================================
// This sample is more of a show-(and test) case for HeapInspector's. It demonstrates:
// 1) That HeapInspector is multithread safe.
// 2) HeapInspector's ability to deal with allocations prior to Initialise (although those allocations will not be tracked).
// 3) HeapInspector's ability to deal with API calls during static initialisation phase.
//
// In this sample, multiple threads are started that perform allocations for a set period of time. The
// application will wait for those threads to finish. After the time is passed and the application calls Shutdown,
// the client will disconnect.
//
// To switch between launching the threads during the static initalisation phase and launching the treads
// in main, flip the INIT_IN_STATIC_PHASE define.
// 
// =================================================================================================================================

#include "IThread.h"
#include <stdlib.h>

void Wait(int a_MilliSeconds);

#define INIT_IN_STATIC_PHASE	0
const int g_NumThreads = 4;

class MultiThreadedAllocator
{
public:
	static void WorkerThread()
	{
		for (int i = 0; i != 1000; ++i)
		{
			void* mem1 = malloc(10);
			Wait(10);
			free(mem1);
			Wait(10);
		}
	}

	MultiThreadedAllocator()
	{
		for (int i = 0; i != g_NumThreads; ++i)
		{
			m_Threads[i] = CreateThread();
			m_Threads[i]->Fork(WorkerThread);
		}
	}

	~MultiThreadedAllocator()
	{
		WaitForThreads();
		for (int i = 0; i != g_NumThreads; ++i)
		{
			DestroyThread(m_Threads[i]);
		}
	}

private:
	void WaitForThreads()
	{
		for (int i = 0; i != g_NumThreads; ++i)
		{
			m_Threads[i]->Join();
		}
	}

private:
	IThread* m_Threads[g_NumThreads];
};

#if INIT_IN_STATIC_PHASE
static MultiThreadedAllocator* g_Allocator = new MultiThreadedAllocator();
#endif

void RunHeapInspectorServer()
{
#if !INIT_IN_STATIC_PHASE
	MultiThreadedAllocator* g_Allocator = new MultiThreadedAllocator();
#endif

	delete g_Allocator;
}
