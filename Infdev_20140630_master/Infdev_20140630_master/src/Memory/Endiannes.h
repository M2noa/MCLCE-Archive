#pragma once

#define BIG_ENDIAN = htonl(47) == 47
#define LITTLE_ENDIAN = 1 - BIG_ENDIAN