#pragma once

class File;

// 4J Jev, java library interface.
class FileFilter
{
public:
	virtual bool accept(File *dir) = 0;
};