#include "stdafx.h"
#include "DefaultTexturePack.h"
#include "Textures.h"
#include "..\Minecraft.World\StringHelpers.h"


DefaultTexturePack::DefaultTexturePack() : AbstractTexturePack(0, NULL, L"Minecraft", NULL)
{
	// 4J Stu - These calls need to be in the most derived version of the class
	loadIcon();
	loadName(); // 4J-PB - added so the PS3 can have localised texture names'
	loadDescription();
	loadColourTable();
}

void DefaultTexturePack::loadIcon()
{
	if(app.hasArchiveFile(L"Graphics\\TexturePackIcon.png"))
	{
		byteArray ba = app.getArchiveFile(L"Graphics\\TexturePackIcon.png");
		m_iconData = ba.data;
		m_iconSize = ba.length;
	}
}

void DefaultTexturePack::loadDescription()
{
	desc1 = L"LOCALISE ME: The default look of Minecraft";
}
void DefaultTexturePack::loadName()
{
	texname = L"Minecraft";
}

bool DefaultTexturePack::hasFile(const wstring &name)
{
//	return DefaultTexturePack::class->getResourceAsStream(name) != null;
	return true;
}

bool DefaultTexturePack::isTerrainUpdateCompatible()
{
	return true;
}

InputStream *DefaultTexturePack::getResourceImplementation(const wstring &name)// throws FileNotFoundException
{
	wstring wDrive = L"";
	// Make the content package point to to the UPDATE: drive is needed
#if __PS3__

	char *pchUsrDir;	
	if(app.GetBootedFromDiscPatch())
	{
		const char *pchTextureName=wstringtofilename(name);
		pchUsrDir = app.GetBDUsrDirPath(pchTextureName);
		app.DebugPrintf("DefaultTexturePack::getResourceImplementation - texture %s - Drive - %s\n",pchTextureName,pchUsrDir);
	}
	else
	{
		const char *pchTextureName=wstringtofilename(name);
		pchUsrDir=getUsrDirPath();
		app.DebugPrintf("DefaultTexturePack::getResourceImplementation - texture %s - Drive - %s\n",pchTextureName,pchUsrDir);
	}


	wstring wstr (pchUsrDir, pchUsrDir+strlen(pchUsrDir));

	wDrive = wstr + L"\\Common\\res\\TitleUpdate\\res";
#else
	wDrive = L"Common\\res\\TitleUpdate\\res";

#endif
	InputStream *resource = InputStream::getResourceAsStream(wDrive + name);
	//InputStream *stream = DefaultTexturePack::class->getResourceAsStream(name);
	//if (stream == NULL)
	//{
	//	throw new FileNotFoundException(name);
	//}

	//return stream;
	return resource;
}

void DefaultTexturePack::loadUI()
{
	loadDefaultUI();

	AbstractTexturePack::loadUI();
}

void DefaultTexturePack::unloadUI()
{
	AbstractTexturePack::unloadUI();
}
