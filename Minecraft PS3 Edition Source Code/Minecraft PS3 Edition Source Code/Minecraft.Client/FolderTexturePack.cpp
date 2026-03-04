#include "stdafx.h"
#include "FolderTexturePack.h"

FolderTexturePack::FolderTexturePack(DWORD id, const wstring &name, File *folder, TexturePack *fallback) : AbstractTexturePack(id, folder, name, fallback)
{
	// 4J Stu - These calls need to be in the most derived version of the class
	loadIcon();
	loadName();
	loadDescription();

	bUILoaded = false;
}

InputStream *FolderTexturePack::getResourceImplementation(const wstring &name) //throws IOException
{
#if 0
	final File file = new File(this.file, name.substring(1));
	if (!file.exists()) {
		throw new FileNotFoundException(name);
	}

	return new BufferedInputStream(new FileInputStream(file));
#endif

	wstring wDrive = L"";
	// Make the content package point to to the UPDATE: drive is needed
	wDrive = L"Common\\DummyTexturePack\\res";
	InputStream *resource = InputStream::getResourceAsStream(wDrive + name);
	//InputStream *stream = DefaultTexturePack::class->getResourceAsStream(name);
	//if (stream == NULL)
	//{
	//	throw new FileNotFoundException(name);
	//}

	//return stream;
	return resource;
}

bool FolderTexturePack::hasFile(const wstring &name)
{
	File file = File( getPath() + name);
	return file.exists() && file.isFile();
	//return true;
}

bool FolderTexturePack::isTerrainUpdateCompatible()
{
#if 0
	final File dir = new File(this.file, "textures/");
	final boolean hasTexturesFolder = dir.exists() && dir.isDirectory();
	final boolean hasOldFiles = hasFile("terrain.png") || hasFile("gui/items.png");
	return hasTexturesFolder || !hasOldFiles;
#endif
	return true;
}

wstring FolderTexturePack::getPath(bool bTitleUpdateTexture /*= false*/)
{
	wstring wDrive;
		wDrive=L"Common\\" + file->getPath() + L"\\";
	return wDrive;
}

void FolderTexturePack::loadUI()
{
}

void FolderTexturePack::unloadUI()
{
}