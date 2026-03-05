
#include "RakDebug.h"
#include <stdio.h>

namespace RakNet
{
	int(*rakDebugLogCallback)(const char*, ...) = printf;
}