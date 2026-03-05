#pragma once
#if 0
#include <np.h>

class CTSS
{

public:

	CTSS(void);
	~CTSS(void);

	int32_t doLookupTitleSmallStorage(void);

private:
	static int32_t	TssVersionCheck(void *data, size_t size);
	static int32_t	TssHashCheck(void *data, size_t size);
	static int32_t	TssSizeCheck(size_t	size);

	int32_t	m_lookupTitleCtxId;
	int32_t m_tusTitleCtxId;
};

#endif
