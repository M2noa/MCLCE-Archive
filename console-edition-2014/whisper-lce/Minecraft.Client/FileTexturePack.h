#pragma once
#include "AbstractTexturePack.h"
//class ZipFile;
class BufferedImage;
class File;
class Textures;
using namespace std;

class FileTexturePack : public AbstractTexturePack
{
private:
	//ZipFile *zipFile;

public:
	FileTexturePack(DWORD id, File *file, TexturePack *fallback);

	//@Override
	void unload(Textures *textures);

protected:
	InputStream *getResourceImplementation(const wstring &name); //throws IOException

public:
	//@Override
	bool hasFile(const wstring &name);

private:
	void loadZipFile(); //throws IOException

public:
	bool isTerrainUpdateCompatible();
};
