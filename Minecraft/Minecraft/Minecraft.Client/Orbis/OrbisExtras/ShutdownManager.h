#pragma once

class ShutdownManager
{
public:
	typedef enum
	{
		eMainThread,

		eLeaderboardThread,
		eCommerceThread,
		ePostProcessThread,
		eRunUpdateThread,
		eRenderChunkUpdateThread,
		eServerThread,
		eStorageManagerThreads,
		eConnectionReadThreads,
		eConnectionWriteThreads,
		eEventQueueThreads,

		eThreadIdCount
	} EThreadId;

	static void Initialise() {}
	static void StartShutdown() {}
	static void MainThreadHandleShutdown() {}
#ifdef __PS3__
	static void SysUtilCallback(uint64_t status, uint64_t param, void *userdata);
#endif

	static void HasStarted(EThreadId threadId) {};
	static void HasStarted(EThreadId threadId, C4JThread::EventArray *eventArray) {};
	static bool ShouldRun(EThreadId threadId) {return true;};
	static void HasFinished(EThreadId threadId) {};

private:
#ifdef __PS3__
	static bool s_threadShouldRun[eThreadIdCount];
	static int s_threadRunning[eThreadIdCount];
	static CRITICAL_SECTION s_threadRunningCS;
	static C4JThread::EventArray *s_eventArray[eThreadIdCount];

	static void RequestThreadToStop(int i);
	static void WaitForSignalledToComplete();
	static void StorageManagerCompleteFn();
#endif
};
