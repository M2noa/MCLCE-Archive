#include "stdafx.h"
#include "LevelConflictException.h"

LevelConflictException::LevelConflictException(const wstring& msg) : RuntimeException(msg)
{
}