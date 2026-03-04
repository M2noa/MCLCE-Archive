#pragma once
#include <queue>

typedef int (C4JThreadStartFunc)(void* lpThreadParameter);

class Level;


#define CPU_CORE_MAIN_THREAD 0

#define CPU_CORE_CHUNK_REBUILD_A 1
#define CPU_CORE_SAVE_THREAD_A 1
#define CPU_CORE_TILE_UPDATE 1
#define CPU_CORE_CONNECTIONS 1

#define CPU_CORE_CHUNK_UPDATE 2
#define CPU_CORE_REMOVE_PLAYER 2

#define CPU_CORE_CHUNK_REBUILD_B 3
#define CPU_CORE_SAVE_THREAD_B 3
#define CPU_CORE_UI_SCENE 3
#define CPU_CORE_POST_PROCESSING 3

#define CPU_CORE_SERVER 4

#define CPU_CORE_CHUNK_REBUILD_C 5
#define CPU_CORE_SAVE_THREAD_C 5
#define CPU_CORE_LEADERBOARDS 5 // Sony only


class C4JThread
{
public:

	class Event
	{
	public:
		enum EMode
		{
			e_modeAutoClear,
			e_modeManualClear
		};
		Event(EMode mode = e_modeAutoClear);	
		~Event();
		void Set();
		void Clear();
		DWORD WaitForSignal(int timeoutMs);

	private:
		EMode	m_mode;
#if defined(__PS3__)
		sys_event_flag_t m_event; 
#else
		HANDLE m_event;
#endif
	};

	class EventArray
	{
	public:
		enum EMode
		{
			e_modeAutoClear,
			e_modeManualClear
		};

		EventArray(int size, EMode mode = e_modeAutoClear);

		void Set(int index);
		void Clear(int index);
		void SetAll();
		void ClearAll();
		DWORD WaitForAll(int timeoutMs);
		DWORD WaitForAny(int timeoutMs);
		DWORD WaitForSingle(int index, int timeoutMs);
#ifdef __PS3__
		void Cancel();
#endif

	private:
		int		m_size;
		EMode	m_mode;
#if defined(__PS3__)
		sys_event_flag_t	m_events; 
#else
		HANDLE*				m_events;
#endif
	};



	class EventQueue
	{
		typedef void (UpdateFunc)(void* lpParameter);
		typedef void (ThreadInitFunc)();

		C4JThread*				m_thread;
		std::queue<void*>		m_queue; 
		C4JThread::EventArray*	m_startEvent;
		C4JThread::Event*		m_finishedEvent;
		CRITICAL_SECTION		m_critSect;
		UpdateFunc*				m_updateFunc;
		ThreadInitFunc*			m_threadInitFunc;
		char					m_threadName[64];
		int						m_processor;
		int						m_priority;
		void init();
		static int threadFunc(void* lpParam);
		void threadPoll();

	public:
		EventQueue(UpdateFunc* updateFunc, ThreadInitFunc threadInitFunc, const char* szThreadName);
		void setProcessor(int proc) { m_processor = proc; if(m_thread) m_thread->SetProcessor(proc); }
		void setPriority(int priority) { m_priority = priority; if(m_thread) m_thread->SetPriority(priority); }
		void sendEvent(Level* pLevel);
		void waitForFinish();
	};



	C4JThread(C4JThreadStartFunc* startFunc, void* param, const char* threadName, int stackSize = 0);
	C4JThread( const char* mainThreadName ); // only used for the main thread
	~C4JThread();

	void Run();
	bool isRunning() { return m_isRunning; }
	bool hasStarted() { return m_hasStarted; }
	void SetProcessor(int proc);
	void SetPriority(int priority);
	DWORD WaitForCompletion(int timeoutMs);
	int GetExitCode();
	char* getName() { return m_threadName; }
	static void Sleep(int millisecs);
	static C4JThread* getCurrentThread();
	static bool isMainThread();
	static char* getCurrentThreadName() { return getCurrentThread()->getName(); }



private:
	void*				m_threadParam;
	C4JThreadStartFunc*	m_startFunc;
	int					m_stackSize;
	char				m_threadName[64];
	bool				m_isRunning;
	bool				m_hasStarted;
	int					m_exitCode;
	__int64				m_lastSleepTime;
	static std::vector<C4JThread*> ms_threadList;
	static CRITICAL_SECTION ms_threadListCS;

	static C4JThread	m_mainThread;

#if defined(__PS3__)
	sys_ppu_thread_t m_threadID;
	Event			*m_completionFlag;
	int				m_priority;
	static void entryPoint(uint64_t);
#else
	DWORD m_threadID;
	HANDLE m_threadHandle;
	Event			*m_completionFlag;
	static DWORD WINAPI	entryPoint(LPVOID lpParam);
#endif
};
void SetThreadName( DWORD dwThreadID, LPCSTR szThreadName );

