#include "stdafx.h"
#include "Textures.h"
#include "AbstractTexturePack.h"
#include "..\Minecraft.World\InputOutputStream.h"
#include "..\Minecraft.World\StringHelpers.h"

AbstractTexturePack::AbstractTexturePack(DWORD id, File *file, const wstring &name, TexturePack *fallback) : id(id), name(name)
{
	// 4J init
	textureId = -1;
	m_colourTable = NULL;


	this->file = file;
	this->fallback = fallback;

	m_iconData = NULL;
	m_iconSize = 0;

	m_comparisonData = NULL;
	m_comparisonSize = 0;

	// 4J Stu - These calls need to be in the most derived version of the class
	//loadIcon();
	//loadDescription();
}

wstring AbstractTexturePack::trim(wstring line)
{
	if (!line.empty() && line.length() > 34)
	{
		line = line.substr(0, 34);
	}
	return line;
}

void AbstractTexturePack::loadIcon()
{
}

void AbstractTexturePack::loadComparison()
{
}

void AbstractTexturePack::loadDescription()
{
	// 4J Unused currently
#if 0
	InputStream *inputStream = NULL;
	BufferedReader *br = NULL;
	//try {
	inputStream = getResourceImplementation(L"/pack.txt");
	br = new BufferedReader(new InputStreamReader(inputStream));
	desc1 = trim(br->readLine());
	desc2 = trim(br->readLine());
	//} catch (IOException ignored) {
	//} finally {
	// TODO [EB]: use IOUtils.closeSilently()
	//	try {
	if (br != NULL)
	{
		br->close();
		delete br;
	}
	if (inputStream != NULL)
	{
		inputStream->close();
		delete inputStream;
	}
	//	} catch (IOException ignored) {
	//	}
	//}
#endif
}

void AbstractTexturePack::loadName()
{
}

InputStream *AbstractTexturePack::getResource(const wstring &name, bool allowFallback) //throws IOException
{
	app.DebugPrintf("texture - %ls\n",name.c_str());
	InputStream *is = getResourceImplementation(name);
	if (is == NULL && fallback != NULL && allowFallback)
	{
		is = fallback->getResource(name, true);
	}

	return is;
}

// 4J Currently removed due to override in TexturePack class
//InputStream *AbstractTexturePack::getResource(const wstring &name) //throws IOException
//{
//	return getResource(name, true);
//}

void AbstractTexturePack::unload(Textures *textures)
{
	if (iconImage != NULL && textureId != -1)
	{
		textures->releaseTexture(textureId);
	}
}

void AbstractTexturePack::load(Textures *textures)
{
	if (iconImage != NULL)
	{
		if (textureId == -1)
		{
			textureId = textures->getTexture(iconImage);
		}
		glBindTexture(GL_TEXTURE_2D, textureId);
		textures->clearLastBoundId();
	}
	else
	{
		// 4J Stu - Don't do this
		//textures->bindTexture(L"/gui/unknown_pack.png");
	}
}

bool AbstractTexturePack::hasFile(const wstring &name, bool allowFallback)
{
	bool hasFile = this->hasFile(name);

	return !hasFile && (allowFallback && fallback != NULL) ? fallback->hasFile(name, allowFallback) : hasFile;
}

DWORD AbstractTexturePack::getId()
{
	return id;
}

wstring AbstractTexturePack::getName()
{
	return texname;
}

wstring AbstractTexturePack::getWorldName()
{
	return m_wsWorldName;
}

wstring AbstractTexturePack::getDesc1()
{
	return desc1;
}

wstring AbstractTexturePack::getDesc2()
{
	return desc2;
}

wstring AbstractTexturePack::getAnimationString(const wstring &textureName, const wstring &path, bool allowFallback)
{
	return getAnimationString(textureName, path);
}

wstring AbstractTexturePack::getAnimationString(const wstring &textureName, const wstring &path)
{
	wstring animationDefinitionFile = textureName + L".txt";

	bool requiresFallback = !hasFile(L"\\" + textureName + L".png", false);

	InputStream *fileStream = getResource(L"\\" + path + animationDefinitionFile, requiresFallback);

	//Minecraft::getInstance()->getLogger().info("Found animation info for: " + animationDefinitionFile);
#ifndef _CONTENT_PACKAGE
	wprintf(L"Found animation info for: %ls\n", animationDefinitionFile.c_str() );
#endif
	InputStreamReader isr(fileStream);
	BufferedReader br(&isr);
	
	wstring result = L"";

	wstring line = br.readLine();
	while (!line.empty())
	{
		line = trimString(line);
		if (line.length() > 0)
		{
			result.append(L",");
			result.append(line);
		}
		line = br.readLine();
	}
	delete fileStream;

	return result;
}

BufferedImage *AbstractTexturePack::getImageResource(const wstring& File, bool filenameHasExtension /*= false*/, bool bTitleUpdateTexture /*=false*/, const wstring &drive /*=L""*/)
{
	const char *pchTexture=wstringtofilename(File);
	app.DebugPrintf("AbstractTexturePack::getImageResource - %s, drive is %s\n",pchTexture, wstringtofilename(drive));

	return new BufferedImage(TexturePack::getResource(L"/" + File),filenameHasExtension,bTitleUpdateTexture,drive);
}

void AbstractTexturePack::loadDefaultUI()
{
	ui.ReloadSkin();
}

void AbstractTexturePack::loadColourTable()
{
	loadDefaultColourTable();
	loadDefaultHTMLColourTable();
}

void AbstractTexturePack::loadDefaultColourTable()
{
	// Load the file
	File coloursFile(AbstractTexturePack::getPath(true).append(L"res/colours.col"));

	if(coloursFile.exists())
	{
		DWORD dwLength = coloursFile.length();
		byteArray data(dwLength);

		FileInputStream fis(coloursFile);
		fis.read(data,0,dwLength);
		fis.close();
		if(m_colourTable != NULL) delete m_colourTable;
		m_colourTable = new ColourTable(data.data, dwLength);

		delete [] data.data;
	}
	else
	{
		app.DebugPrintf("Failed to load the default colours table\n");
		app.FatalLoadError();
	}
}

void AbstractTexturePack::loadDefaultHTMLColourTable()
{
	if(app.hasArchiveFile(L"HTMLColours.col"))
	{
		byteArray textColours = app.getArchiveFile(L"HTMLColours.col");
		m_colourTable->loadColoursFromData(textColours.data,textColours.length);

		delete [] textColours.data;
	}
}


void AbstractTexturePack::loadUI()
{
	loadColourTable();
	
}

void AbstractTexturePack::unloadUI()
{
	// Do nothing
}

wstring AbstractTexturePack::getXuiRootPath()
{
	const ULONG_PTR c_ModuleHandle = (ULONG_PTR)GetModuleHandle(NULL);

	// Load new skin
	const DWORD LOCATOR_SIZE = 256; // Use this to allocate space to hold a ResourceLocator string 
	WCHAR szResourceLocator[ LOCATOR_SIZE ];

	swprintf(szResourceLocator, LOCATOR_SIZE,L"section://%X,%ls#%ls",c_ModuleHandle,L"media", L"media/");
	return szResourceLocator;
}

PBYTE AbstractTexturePack::getPackIcon(DWORD &dwImageBytes)
{
	if(m_iconSize == 0 || m_iconData == NULL) loadIcon();
	dwImageBytes = m_iconSize;
	return m_iconData;
}

PBYTE AbstractTexturePack::getPackComparison(DWORD &dwImageBytes)
{
	if(m_comparisonSize == 0 || m_comparisonData == NULL) loadComparison();

	dwImageBytes = m_comparisonSize;
	return m_comparisonData;
}

unsigned int AbstractTexturePack::getDLCParentPackId()
{
	return 0;
}

unsigned char AbstractTexturePack::getDLCSubPackId()
{
	return 0;
}