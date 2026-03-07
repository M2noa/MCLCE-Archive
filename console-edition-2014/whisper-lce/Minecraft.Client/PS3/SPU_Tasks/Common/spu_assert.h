#pragma once



#ifdef _CONTENT_PACKAGE

#define spu_assert(a)  {}
#define spu_print(...) {}

#else

#include <spu_printf.h>
#define spu_print spu_printf
#define spu_assert(a)	{ \
							if(!(a)) \
							{ \
							spu_printf(	"===================================\n" \
										"spu_assert : \t%s \n      Func : \t%s \n      File : \t%s \n      Line : \t%d\n" \
										"===================================\n", #a, __PRETTY_FUNCTION__, __FILE__, __LINE__); \
							si_stop(2); \
							} \
						}

#endif // _CONTENT_PACKAGE