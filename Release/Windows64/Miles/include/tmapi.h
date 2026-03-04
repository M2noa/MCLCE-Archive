// This is the null header file used to remove Telemetry calls.

#define TMERR_DISABLED 1

#define tmTick(...)
#define tmPause(...)
#define tmEnter(...)
#define tmLeave(...)
#define tmThreadName(...) TMERR_DISABLED
#define tmMutexName(...) TMERR_DISABLED
#define tmTryLock(...) TMERR_DISABLED
#define tmEndTryLock(...)
#define tmSetMutexState(...)
#define tmAlloc(...)
#define tmRealloc(...)
#define tmFree(...) 
#define tmPlot(...)
#define tmBlob(...) TMERR_DISABLED
#define tmBlobEx(...) TMERR_DISABLED
#define tmMessage(...)
#define tmEmitAccumulationZones(...) TMERR_DISABLED
#define tmEnterAccumulationZone(...) TMERR_DISABLED
#define tmLeaveAccumulationZone(...) TMERR_DISABLED
#define tmZone(...) 
#define tmSetLockState(...)
#define tmLockName(...)
#define tmSendCallStack(...)
#define tmAllocEx(...)

#define NTELEMETRY 1

#define TM_CONTEXT_LITE(val) ((char*)(val))
#define TM_CONTEXT_FULL(val) ((char*)(val))

typedef char *HTELEMETRY;

