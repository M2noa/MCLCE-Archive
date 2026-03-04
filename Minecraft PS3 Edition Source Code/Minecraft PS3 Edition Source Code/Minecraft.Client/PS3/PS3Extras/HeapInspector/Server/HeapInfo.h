#ifndef _HEAPINSPECTORSERVER_HEAPINFO_H_
#define _HEAPINSPECTORSERVER_HEAPINFO_H_

#include "HeapInspectorServerTypes.h"
#include <string>

BEGIN_NAMESPACE(HeapInspectorServer)

struct Range
{
	uint32 m_Min;
	uint32 m_Max;
};

struct HeapInfo
{
	std::string m_Description;
	Range m_Range;
};

END_NAMESPACE(HeapInspectorServer)

#endif // _HEAPINSPECTORSERVER_HEAPINFO_H_