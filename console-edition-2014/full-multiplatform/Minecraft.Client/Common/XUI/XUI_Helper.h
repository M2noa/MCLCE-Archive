#pragma once

#define BEGIN_CONTROL_MAP() \
	HRESULT MapChildControls() \
	{ \
		HRESULT hr = S_OK; \
		CXuiElement e = m_hObj; \



#define MAP_CONTROL(name, member) \
	hr = e.GetChildById(name, &member); \
	assert(hr==0); \

#define BEGIN_MAP_CHILD_CONTROLS( member ) \
	{ \
		CXuiElement tempE = e; \
		e = member; \

#define END_MAP_CHILD_CONTROLS() \
		e = tempE; \
	} \


#define MAP_OVERRIDE(name, member) \
	{ \
		HXUIOBJ h; \
		hr = e.GetChildById(name, &h); \
		assert(hr==0); \
		hr = XuiObjectFromHandle(h, reinterpret_cast<PVOID*>(&member)); \
		assert(hr==0); \
	} \


#define END_CONTROL_MAP() \
	return hr; \
	} \

