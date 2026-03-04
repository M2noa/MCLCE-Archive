#pragma once
#include "File.h"
class FileEntry;
class ConsoleSaveFile;
class ProgressRenderer;

// 4J Stu - This code is taken from the standalone save converter tool, and modified slightly
class ConsoleSaveFileConverter
{
private:
	static void ProcessSimpleFile(ConsoleSaveFile *sourceSave, FileEntry *sourceFileEntry, ConsoleSaveFile *targetSave, FileEntry *targetFileEntry);
	static void ProcessStandardRegionFile(ConsoleSaveFile *sourceSave, File sourceFile, ConsoleSaveFile *targetSave, File targetFile);

public:
	static void ConvertSave(ConsoleSaveFile *sourceSave, ConsoleSaveFile *targetSave, ProgressListener *progress);
};