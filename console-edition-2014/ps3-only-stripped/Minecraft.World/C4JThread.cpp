#include "stdafx.h"


#include "C4JThread.h"
#include "..\Minecraft.Client\PS3\PS3Extras\ShutdownManager.h"


std::vector<C4JThread*> C4JThread::ms_threadList;
CRITICAL_SECTION C4JThread::ms_threadListCS;

C4JThread	C4JThread::m_mainThread("Main thread");




C4JThread::C4JThread( C4JThreadStartFunc* startFunc, void* param, const char* threadName, int stackSize/* = 0*/ )
{
	m_startFunc = startFunc;
	m_threadParam = param;
	m_stackSize = stackSize;

	// to match XBox, if the stack size is zero, use the default 64k
	if(m_stackSize == 0)
		m_stackSize = 65536 * 2;
	// make sure it's at least 16K
	if(m_stackSize < 16384)
		m_stackSize = 16384;

#ifdef __PS3__
	sprintf(m_threadName, "(4J) %s", threadName );
#else
	sprintf_s(m_threadName,64, "(4J) %s", threadName );
#endif

	m_isRunning = false;
	m_hasStarted = false;

	m_exitCode = STILL_ACTIVE;

#if defined(__PS3__)
	m_completionFlag = new Event(Event::e_modeManualClear);
	m_threadID = 0;
	m_lastSleepTime = 0;
	m_priority = 1002;	// main thread has priority 1001
#else
	m_threadID = 0;
	m_threadHandle = 0;
	m_threadHandle = CreateThread(NULL, m_stackSize, entryPoint, this, CREATE_SUSPENDED, &m_threadID);
#endif
	EnterCriticalSection(&ms_threadListCS);
	ms_threadList.push_back(this);
	LeaveCriticalSection(&ms_threadListCS);
}

// only used for the main thread
C4JThread::C4JThread( const char* mainThreadName)
{

	m_startFunc = NULL;
	m_threadParam = NULL;
	m_stackSize = 0;

#ifdef __PS3__
	sprintf(m_threadName, "(4J) %s", mainThreadName);
#else
	sprintf_s(m_threadName, 64, "(4J) %s", mainThreadName);
#endif
	m_isRunning = true;
	m_hasStarted = true;
	m_lastSleepTime = System::currentTimeMillis();

	// should be the first thread to be created, so init the static critical section for the threadlist here
	InitializeCriticalSection(&ms_threadListCS);


#if defined(__PS3__)
	m_completionFlag = new Event(Event::e_modeManualClear);
	sys_ppu_thread_get_id(&m_threadID);
#else
	m_threadID = GetCurrentThreadId();
	m_threadHandle = GetCurrentThread();
#endif
	EnterCriticalSection(&ms_threadListCS);
	ms_threadList.push_back(this);
	LeaveCriticalSection(&ms_threadListCS);
}

C4JThread::~C4JThread()
{
#if defined __PS3__
	delete m_completionFlag;
#endif


	EnterCriticalSection(&ms_threadListCS);

	for( AUTO_VAR(it,ms_threadList.begin()); it != ms_threadList.end(); it++ )
	{
		if( (*it) == this )
		{
			ms_threadList.erase(it);			
			LeaveCriticalSection(&ms_threadListCS);
			return;
		}
	}

	LeaveCriticalSection(&ms_threadListCS);
}

#if defined(__PS3__)
void C4JThread::entryPoint(uint64_t param)
{
	C4JThread* pThread = (C4JThread*)param;
	pThread->m_exitCode = (*pThread->m_startFunc)(pThread->m_threadParam);
	pThread->m_completionFlag->Set();
	pThread->m_isRunning = false;
	sys_ppu_thread_exit(0);
}
#else
DWORD WINAPI	C4JThread::entryPoint(LPVOID lpParam)
{
	C4JThread* pThread = (C4JThread*)lpParam;
	SetThreadName(-1, pThread->m_threadName);
	pThread->m_exitCode = (*pThread->m_startFunc)(pThread->m_threadParam);
	pThread->m_isRunning = false;
	return pThread->m_exitCode;
}
#endif




void C4JThread::Run()
{
#if defined(__PS3__)
	//		prio specifies the priority value of the PPU thread within the range from 0 to 3071 where 0 is the highest.
	// One of the following values is set to flags:
	// 0 - non-joinable non-interrupt thread 
	// SYS_PPU_THREAD_CREATE_JOINABLE - Create a joinable thread 
	// SYS_PPU_THREAD_CREATE_INTERRUPT - Create an interrupt thread 
	uint64_t flags = 0;
	int err = sys_ppu_thread_create(&m_threadID, entryPoint, (uint64_t)this, m_priority, m_stackSize, flags, m_threadName);
#else
	ResumeThread(m_threadHandle);
#endif
	m_lastSleepTime = System::currentTimeMillis();
	m_isRunning = true;
	m_hasStarted = true;
}

void C4JThread::SetProcessor( int proc )
{
#if defined(__PS3__)
	// does nothing since we only have the 1 processor
#else
	XSetThreadProcessor( m_threadHandle, proc);
#endif
}

void C4JThread::SetPriority( int priority )
{
#if defined(__PS3__)
	switch(priority)
	{
	case THREAD_PRIORITY_LOWEST:			m_priority = 1003; break;  
	case THREAD_PRIORITY_BELOW_NORMAL:		m_priority = 1002; break;  
	case THREAD_PRIORITY_NORMAL:			m_priority = 1001; break;  // same as main thread
	case THREAD_PRIORITY_ABOVE_NORMAL:		m_priority = 1000; break; 
	case THREAD_PRIORITY_HIGHEST:			m_priority = 999; break; 
	}
	if(m_threadID != 0)
		sys_ppu_thread_set_priority(m_threadID, m_priority);
	//int erro = sys_ppu_thread_set_priority(m_threadID, priority);
#else
	SetThreadPriority(m_threadHandle, priority);
#endif
}

DWORD C4JThread::WaitForCompletion( int timeoutMs )
{
#if defined(__PS3__)
	if(timeoutMs == INFINITE)
		timeoutMs = SYS_NO_TIMEOUT ;
	return m_completionFlag->WaitForSignal(timeoutMs);
#else
	return WaitForSingleObject(m_threadHandle, timeoutMs);
#endif
}

int C4JThread::GetExitCode()
{
#if defined  __PS3__
	return m_exitCode;
#else
	DWORD exitcode = 0;
	GetExitCodeThread(m_threadHandle, &exitcode);

	return *((int *)&exitcode);
#endif
}

void C4JThread::Sleep( int millisecs )
{
#if defined(__PS3__)
	if(millisecs == 0)
	{
		// https://ps3.scedev.net/forums/thread/116470/
		// "sys_timer_usleep(0) does not yield the CPU."
		sys_ppu_thread_yield();
	}
	else
		sys_timer_usleep(millisecs * 1000);
#else
	::Sleep(millisecs);
#endif
}

C4JThread* C4JThread::getCurrentThread()
{
#if defined(__PS3__)
	sys_ppu_thread_t currThreadID;
	sys_ppu_thread_get_id(&currThreadID);
#else
	DWORD currThreadID = GetCurrentThreadId();
#endif
	EnterCriticalSection(&ms_threadListCS);

	for(int i=0;i<ms_threadList.size(); i++)
	{
		if(currThreadID == ms_threadList[i]->m_threadID)
		{
			LeaveCriticalSection(&ms_threadListCS);
			return ms_threadList[i];
		}
	}

	LeaveCriticalSection(&ms_threadListCS);

	return NULL;
}

bool C4JThread::isMainThread()
{
	return getCurrentThread() == &m_mainThread;
}

C4JThread::Event::Event(EMode mode/* = e_modeAutoClear*/)
{
	m_mode = mode;
#if defined(__PS3__)
	sys_event_flag_attribute_t attr;
	// default values taken from sys_event_flag_attribute_initialize
	attr.attr_protocol = SYS_SYNC_PRIORITY;
	attr.attr_pshared = SYS_SYNC_NOT_PROCESS_SHARED;
	attr.key = 0;
	attr.flags = 0;
	attr.type = SYS_SYNC_WAITER_SINGLE; 
	attr.name[0] = '\0';
	sys_event_flag_attribute_initialize(attr);
	
	int err = sys_event_flag_create(&m_event, &attr, 0);

#else
	m_event = CreateEvent( NULL, (m_mode == e_modeManualClear), FALSE, NULL );
#endif
}


C4JThread::Event::~Event()
{
#if defined(__PS3__)
	sys_event_flag_destroy(m_event);
#else
	CloseHandle( m_event );
#endif
}


void C4JThread::Event::Set()
{
#if defined(__PS3__)
	int err =sys_event_flag_set(m_event, 1);
#else
	SetEvent(m_event);
#endif
}

void C4JThread::Event::Clear()
{
#if defined(__PS3__)
	int err =sys_event_flag_clear(m_event, ~(1));
#else
	ResetEvent(m_event);
#endif
}

DWORD C4JThread::Event::WaitForSignal( int timeoutMs )
{
#if defined(__PS3__)
	if(timeoutMs == INFINITE)
		timeoutMs = SYS_NO_TIMEOUT ;
	int timoutMicrosecs = timeoutMs * 1000;
	uint32_t mode = SYS_EVENT_FLAG_WAIT_AND;
	if(m_mode == e_modeAutoClear)
		mode |= SYS_EVENT_FLAG_WAIT_CLEAR;
	int err = sys_event_flag_wait(m_event, 1, mode, 0, timoutMicrosecs);

	switch(err)
	{
	case CELL_OK: return WAIT_OBJECT_0;
	case ETIMEDOUT: return WAIT_TIMEOUT;
	case ECANCELED: return WAIT_ABANDONED;
	default: return WAIT_FAILED;
	}

#else
	return WaitForSingleObject(m_event, timeoutMs);
#endif
}

C4JThread::EventArray::EventArray( int size, EMode mode/* = e_modeAutoClear*/)
{
	assert(size<32);
	m_size = size;
	m_mode = mode;
#if defined(__PS3__)
	sys_event_flag_attribute_t attr;
	// default values taken from sys_event_flag_attribute_initialize
	attr.attr_protocol = SYS_SYNC_PRIORITY;
	attr.attr_pshared = SYS_SYNC_NOT_PROCESS_SHARED;
	attr.key = 0;
	attr.flags = 0;
	attr.type = SYS_SYNC_WAITER_SINGLE; 
	attr.name[0] = '\0';
	sys_event_flag_attribute_initialize(attr);
	int err = sys_event_flag_create(&m_events, &attr, 0);
	assert(err == CELL_OK);
#else
	m_events = new HANDLE[size];
	for(int i=0;i<size;i++)
	{
		m_events[i]  = CreateEvent(NULL, (m_mode == e_modeManualClear), FALSE, NULL );
	}
#endif
}


void C4JThread::EventArray::Set(int index)
{
#if defined(__PS3__)
	int err =sys_event_flag_set(m_events, 1<<index);
	assert(err == CELL_OK);
#else
	SetEvent(m_events[index]);
#endif
}

void C4JThread::EventArray::Clear(int index)
{
#if defined(__PS3__)
	int err =sys_event_flag_clear(m_events, ~(1<<index));
	assert(err == CELL_OK);
#else
	ResetEvent(m_events[index]);
#endif
}

void C4JThread::EventArray::SetAll()
{
	for(int i=0;i<m_size;i++)
		Set(i);
}

void C4JThread::EventArray::ClearAll()
{
	for(int i=0;i<m_size;i++)
		Clear(i);
}

DWORD C4JThread::EventArray::WaitForSingle(int index, int timeoutMs )
{
	DWORD retVal;
#if defined(__PS3__)
	int timeoutMicrosecs;
	if(timeoutMs == INFINITE)
		timeoutMicrosecs = SYS_NO_TIMEOUT;
	else
		timeoutMicrosecs = timeoutMs * 1000;
	uint32_t mode = SYS_EVENT_FLAG_WAIT_AND;
	if(m_mode == e_modeAutoClear)
		mode |= SYS_EVENT_FLAG_WAIT_CLEAR;

	int err = sys_event_flag_wait(m_events, 1<<index, mode, 0, timeoutMicrosecs);

	switch(err)
	{
	case CELL_OK:
		retVal = WAIT_OBJECT_0;
		break;
	case ETIMEDOUT:
		retVal = WAIT_TIMEOUT;
		break;
	case ECANCELED:
		retVal = WAIT_ABANDONED;
		break;
	default:
		assert(0);
		retVal = WAIT_FAILED;
		break;
	}
#else
	retVal = WaitForSingleObject(m_events[index], timeoutMs);
#endif

	return retVal;
}

DWORD C4JThread::EventArray::WaitForAll(int timeoutMs )
{
	DWORD retVal;
#if defined(__PS3__)
	if(timeoutMs == INFINITE)
		timeoutMs = SYS_NO_TIMEOUT ;
	int timoutMicrosecs = timeoutMs * 1000;
	unsigned int bitmask = 0;
	for(int i=0;i<m_size;i++)
		bitmask |= (1<<i);

	uint32_t mode = SYS_EVENT_FLAG_WAIT_AND;
	if(m_mode == e_modeAutoClear)
		mode |= SYS_EVENT_FLAG_WAIT_CLEAR;

	int err = sys_event_flag_wait(m_events, bitmask, mode, 0, timoutMicrosecs);

	switch(err)
	{
	case CELL_OK:
		retVal = WAIT_OBJECT_0;
		break;
	case ETIMEDOUT:
		retVal = WAIT_TIMEOUT;
		break;
	case ECANCELED:
		retVal = WAIT_ABANDONED;
		break;
	default:
		assert(0);
		retVal = WAIT_FAILED;
		break;
	}

#else
	retVal = WaitForMultipleObjects(m_size, m_events, true, timeoutMs);
#endif

	return retVal;
}

DWORD C4JThread::EventArray::WaitForAny(int timeoutMs )
{
#if defined(__PS3__)
	if(timeoutMs == INFINITE)
		timeoutMs = SYS_NO_TIMEOUT ;
	int timoutMicrosecs = timeoutMs * 1000;
	unsigned int bitmask = 0;
	for(int i=0;i<m_size;i++)
		bitmask |= (1<<i);

	uint32_t mode = SYS_EVENT_FLAG_WAIT_OR;
	if(m_mode == e_modeAutoClear)
		mode |= SYS_EVENT_FLAG_WAIT_CLEAR;

	int err = sys_event_flag_wait(m_events, bitmask, mode, 0, timoutMicrosecs);

	switch(err)
	{
	case CELL_OK: return WAIT_OBJECT_0;
	case ETIMEDOUT: return WAIT_TIMEOUT;
	case ECANCELED: return WAIT_ABANDONED;
	default:
		assert(0);
		return WAIT_FAILED;
	}

#else
	return WaitForMultipleObjects(m_size, m_events, false, timeoutMs);
#endif
}

#ifdef __PS3__
void C4JThread::EventArray::Cancel()
{
	sys_event_flag_cancel(m_events, NULL);
}
#endif 




C4JThread::EventQueue::EventQueue( UpdateFunc* updateFunc, ThreadInitFunc threadInitFunc, const char* szThreadName)
{
	m_updateFunc = updateFunc;
	m_threadInitFunc = threadInitFunc;
	strcpy(m_threadName, szThreadName);
	m_thread = NULL;
	m_startEvent = NULL;
	m_finishedEvent = NULL;
	m_processor = -1;
	m_priority = THREAD_PRIORITY_HIGHEST+1;
}

void C4JThread::EventQueue::init()
{
	m_startEvent = new C4JThread::EventArray(1);
	m_finishedEvent = new C4JThread::Event();
	InitializeCriticalSection(&m_critSect);
	m_thread = new C4JThread(threadFunc, this, m_threadName);
	if(m_processor >= 0)
		m_thread->SetProcessor(m_processor);
	if(m_priority != THREAD_PRIORITY_HIGHEST+1)
		m_thread->SetPriority(m_priority);
	m_thread->Run();
}

void C4JThread::EventQueue::sendEvent( Level* pLevel )
{
	if(m_thread == NULL)
		init();
	EnterCriticalSection(&m_critSect);
	m_queue.push(pLevel);
	m_startEvent->Set(0);
	m_finishedEvent->Clear();
	LeaveCriticalSection(&m_critSect);
}

void C4JThread::EventQueue::waitForFinish()
{
	if(m_thread == NULL)
		init();
	EnterCriticalSection(&m_critSect);
	if(m_queue.empty())
	{
		LeaveCriticalSection((&m_critSect));
		return;
	}
	LeaveCriticalSection((&m_critSect));
	m_finishedEvent->WaitForSignal(INFINITE);
}

int C4JThread::EventQueue::threadFunc( void* lpParam )
{
	EventQueue* p = (EventQueue*)lpParam;
	p->threadPoll();
	return 0;
}

void C4JThread::EventQueue::threadPoll()
{
	ShutdownManager::HasStarted(ShutdownManager::eEventQueueThreads, m_startEvent);

	if(m_threadInitFunc)
		m_threadInitFunc();

	while(ShutdownManager::ShouldRun(ShutdownManager::eEventQueueThreads))
	{

		DWORD err = m_startEvent->WaitForAny(INFINITE);
		if(err == WAIT_OBJECT_0)
		{
			bool bListEmpty = true;
			do 
			{
				EnterCriticalSection(&m_critSect);
				void* updateParam = m_queue.front();
				LeaveCriticalSection(&m_critSect);

				m_updateFunc(updateParam);

				EnterCriticalSection(&m_critSect);
				m_queue.pop();
				bListEmpty = m_queue.empty();
				if(bListEmpty)
				{
					m_finishedEvent->Set();
				}
				LeaveCriticalSection(&m_critSect);

			} while(!bListEmpty);
		}
	};

	ShutdownManager::HasFinished(ShutdownManager::eEventQueueThreads);
}

