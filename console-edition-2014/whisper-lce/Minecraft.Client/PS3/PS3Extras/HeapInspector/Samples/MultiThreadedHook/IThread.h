#ifndef _MULTITHREADEDHOOKSAMPLE_ITHREAD_H_
#define _MULTITHREADEDHOOKSAMPLE_ITHREAD_H_

typedef void(*ThreadFunction)();

class IThread
{
public:
	virtual ~IThread() {}
	virtual void Fork(ThreadFunction a_Function) = 0;
	virtual void Join() = 0;
};

IThread* CreateThread();
void DestroyThread(IThread* a_Thread);

#endif // _MULTITHREADEDHOOKSAMPLE_ITHREAD_H_