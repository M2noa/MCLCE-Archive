#pragma once

class  ConsoleInputSource
{
public:
	virtual void info(const wstring& string) = 0;
    virtual void warn(const wstring& string) = 0;
    virtual wstring getConsoleName() = 0;
};
