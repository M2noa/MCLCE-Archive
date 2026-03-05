#pragma once
using namespace std;

#include "Exceptions.h"

class LevelConflictException : public RuntimeException
{
private:
	static const __int32 serialVersionUID = 1L;
    
public:
	LevelConflictException(const wstring& msg);
};