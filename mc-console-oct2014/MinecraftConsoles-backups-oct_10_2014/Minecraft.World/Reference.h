#pragma once

template<class T> class Reference
{
private:
	T *obj;
public:
	T *get() { return obj; }
	Reference(T *i) { obj = i; }
};