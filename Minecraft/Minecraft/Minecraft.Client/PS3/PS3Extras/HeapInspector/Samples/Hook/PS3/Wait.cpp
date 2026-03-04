#include <sys/timer.h>

void Wait(int a_Milliseconds)
{
	sys_timer_usleep(a_Milliseconds * 1000);
}