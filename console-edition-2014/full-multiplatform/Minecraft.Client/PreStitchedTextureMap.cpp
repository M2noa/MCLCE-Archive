#include "stdafx.h"
#include "..\Minecraft.World\net.minecraft.world.h"
#include "..\Minecraft.World\net.minecraft.world.level.tile.h"
#include "..\Minecraft.World\net.minecraft.world.item.h"
#include "..\Minecraft.World\ByteBuffer.h"
#include "Minecraft.h"
#include "LevelRenderer.h"
#include "EntityRenderDispatcher.h"
#include "Stitcher.h"
#include "StitchSlot.h"
#include "StitchedTexture.h"
#include "Texture.h"
#include "TextureHolder.h"
#include "TextureManager.h"
#include "TexturePack.h"
#include "TexturePackRepository.h"
#include "PreStitchedTextureMap.h"
#include "SimpleIcon.h"
#include "CompassTexture.h"
#include "ClockTexture.h"

const wstring PreStitchedTextureMap::NAME_MISSING_TEXTURE = L"missingno";

PreStitchedTextureMap::PreStitchedTextureMap(int type, const wstring &name, const wstring &path, BufferedImage *missingTexture, bool mipmap) : iconType(type), name(name), path(path), extension(L".png")
{
	this->missingTexture = missingTexture;

	// 4J Initialisers
	missingPosition = NULL;
	stitchResult = NULL;

	m_mipMap = mipmap;
	missingPosition = (StitchedTexture *)(new SimpleIcon(NAME_MISSING_TEXTURE,0,0,1,1));
}

void PreStitchedTextureMap::stitch()
{
	// Animated StitchedTextures store a vector of textures for each frame of the animation. Free any pre-existing ones here.
	for(AUTO_VAR(it, texturesToAnimate.begin()); it != texturesToAnimate.end(); ++it)
	{
		StitchedTexture *animatedStitchedTexture = (StitchedTexture *)texturesByName[it->first];
		animatedStitchedTexture->freeFrameTextures();
	}

	loadUVs();

	if (iconType == Icon::TYPE_TERRAIN)
	{
		//for (Tile tile : Tile.tiles)
		for(unsigned int i = 0; i < Tile::TILE_NUM_COUNT; ++i)
		{
			if (Tile::tiles[i] != NULL)
			{
				Tile::tiles[i]->registerIcons(this);
			}
		}

		Minecraft::GetInstance()->levelRenderer->registerTextures(this);
		EntityRenderDispatcher::instance->registerTerrainTextures(this);
	}

	//for (Item item : Item.items)
	for(unsigned int i = 0; i < Item::ITEM_NUM_COUNT; ++i)
	{
		Item *item = Item::items[i];
		if (item != NULL && item->getIconType() == iconType)
		{
			item->registerIcons(this);
		}
	}

	// Collection bucket for multiple frames per texture
	unordered_map<TextureHolder *, vector<Texture *> * > textures; // = new HashMap<TextureHolder, List<Texture>>();

	Stitcher *stitcher = TextureManager::getInstance()->createStitcher(name);

	animatedTextures.clear();

	// Create the final image
	wstring filename = name + extension;

	TexturePack *texturePack = Minecraft::GetInstance()->skins->getSelected();
	//try {
	int mode = Texture::TM_DYNAMIC;
	int clamp = Texture::WM_WRAP; // 4J Stu - Don't clamp as it causes issues with how we signal non-mipmmapped textures to the pixel shader //Texture::WM_CLAMP;
	int minFilter = Texture::TFLT_NEAREST;
	int magFilter = Texture::TFLT_NEAREST;

	MemSect(32);
	wstring drive = L"";
	if(texturePack->hasFile(L"res/" + filename,false))
	{
		drive = texturePack->getPath(true);
	}
	else
	{
		drive = Minecraft::GetInstance()->skins->getDefault()->getPath(true);
		texturePack = Minecraft::GetInstance()->skins->getDefault();
	}
	//BufferedImage *image = new BufferedImage(texturePack->getResource(L"/" + filename),false,true,drive); //ImageIO::read(texturePack->getResource(L"/" + filename));
	BufferedImage *image = texturePack->getImageResource(filename, false, true, drive);
	MemSect(0);
	int height = image->getHeight();
	int width = image->getWidth();

	if(stitchResult != NULL)
	{
		TextureManager::getInstance()->unregisterTexture(name, stitchResult);
		delete stitchResult;
	}
	stitchResult = TextureManager::getInstance()->createTexture(name, Texture::TM_DYNAMIC, width, height, Texture::TFMT_RGBA, m_mipMap);
	stitchResult->transferFromImage(image);
	delete image;
	TextureManager::getInstance()->registerName(name, stitchResult);
	//stitchResult = stitcher->constructTexture(m_mipMap);

	for(AUTO_VAR(it, texturesByName.begin()); it != texturesByName.end(); ++it)
	{
		StitchedTexture *preStitched = (StitchedTexture *)it->second;

		int x = preStitched->getU0() * stitchResult->getWidth();
		int y = preStitched->getV0() * stitchResult->getHeight();
		int width = (preStitched->getU1() * stitchResult->getWidth()) - x;
		int height = (preStitched->getV1() * stitchResult->getHeight()) - y;

		preStitched->init(stitchResult, NULL, x, y, width, height, false);
	}

	MemSect(52);
	for(AUTO_VAR(it, texturesToAnimate.begin()); it != texturesToAnimate.end(); ++it)
	{
		wstring textureName = it->first;
		wstring textureFileName = it->second;

		StitchedTexture *preStitched = (StitchedTexture *)texturesByName[textureName];

		if(!preStitched->hasOwnData())
		{
			if(preStitched->getFrames() > 1) animatedTextures.push_back(preStitched);
			continue;
		}

		wstring filename = path + textureFileName + extension;

		// TODO: [EB] Put the frames into a proper object, not this inside out hack
		vector<Texture *> *frames = TextureManager::getInstance()->createTextures(filename, m_mipMap);
		if (frames == NULL || frames->empty())
		{
			continue; // Couldn't load a texture, skip it
		}

		Texture *first = frames->at(0);

#ifndef _CONTENT_PACKAGE
		if(first->getWidth() != preStitched->getWidth() || first->getHeight() != preStitched->getHeight())
		{
			__debugbreak();
		}
#endif

		preStitched->init(stitchResult, frames, preStitched->getX(), preStitched->getY(), first->getWidth(), first->getHeight(), false);

		if (frames->size() > 1)
		{
			animatedTextures.push_back(preStitched);

			wstring animString = texturePack->getAnimationString(textureFileName, path, true);

			preStitched->loadAnimationFrames(animString);
		}
	}
	MemSect(0);
	//missingPosition = (StitchedTexture *)texturesByName.find(NAME_MISSING_TEXTURE)->second;

	stitchResult->writeAsPNG(L"debug.stitched_" + name + L".png");
	stitchResult->updateOnGPU();


#ifdef __PSVITA__
	// AP - alpha cut out is expensive on vita so we mark which icons actually require it
	DWORD *data = (DWORD*) this->getStitchedTexture()->getData()->getBuffer();
	int Width = this->getStitchedTexture()->getWidth();
	int Height = this->getStitchedTexture()->getHeight();
	for(AUTO_VAR(it, texturesByName.begin()); it != texturesByName.end(); ++it)
	{
		StitchedTexture *preStitched = (StitchedTexture *)it->second;

		bool Found = false;
		int u0 = preStitched->getU0() * Width;
		int u1 = preStitched->getU1() * Width;
		int v0 = preStitched->getV0() * Height;
		int v1 = preStitched->getV1() * Height;

		// check all the texels for this icon. If ANY are transparent we mark it as 'cut out'
		for( int v = v0;v < v1; v+= 1 )
		{
			for( int u = u0;u < u1; u+= 1 )
			{
				// is this texel alpha value < 0.1
				if( (data[v * Width + u] & 0xff000000) < 0x20000000 )
				{
					// this texel is transparent. Mark the icon as such and bail
					preStitched->setFlags(Icon::IS_ALPHA_CUT_OUT);
					Found = true;
					break;
				}
			}

			if( Found )
			{
				// move onto the next icon
				break;
			}
		}
	}
#endif
}

StitchedTexture *PreStitchedTextureMap::getTexture(const wstring &name)
{
#ifndef _CONTENT_PACKAGE
	app.DebugPrintf("Not implemented!\n");
	__debugbreak();
#endif
	return NULL;
#if 0
	StitchedTexture *result = texturesByName.find(name)->second;
	if (result == NULL) result = missingPosition;
	return result;
#endif
}

void PreStitchedTextureMap::cycleAnimationFrames()
{
	//for (StitchedTexture texture : animatedTextures)
	for(AUTO_VAR(it, animatedTextures.begin() ); it != animatedTextures.end(); ++it)
	{
		StitchedTexture *texture = *it;
		texture->cycleFrames();
	}
}

Texture *PreStitchedTextureMap::getStitchedTexture()
{
	return stitchResult;
}

// 4J Stu - register is a reserved keyword in C++
Icon *PreStitchedTextureMap::registerIcon(const wstring &name)
{
	Icon *result = NULL;
	if (name.empty())
	{
		app.DebugPrintf("Don't register NULL\n");
#ifndef _CONTENT_PACKAGE
		__debugbreak();
#endif
		result = missingPosition;
		//new RuntimeException("Don't register null!").printStackTrace();
	}

	AUTO_VAR(it, texturesByName.find(name));
	if(it != texturesByName.end()) result = it->second;

	if (result == NULL)
	{
#ifndef _CONTENT_PACKAGE
		wprintf(L"Could not find uv data for icon %ls\n", name.c_str() );
		__debugbreak();
#endif
		result = missingPosition;
	}

	return result;
}

int PreStitchedTextureMap::getIconType()
{
	return iconType;
}

Icon *PreStitchedTextureMap::getMissingIcon()
{
	return missingPosition;
}

void PreStitchedTextureMap::loadUVs()
{
	if(!texturesByName.empty())
	{
		// 4J Stu - We only need to populate this once at the moment as we have hardcoded positions for each texture
		// If we ever load that dynamically, be aware that the Icon objects could currently be being used by the
		// GameRenderer::runUpdate thread
		return;
	}

	for(AUTO_VAR(it, texturesByName.begin()); it != texturesByName.end(); ++it)
	{
		delete it->second;
	}
	texturesByName.clear();
	texturesToAnimate.clear();

	float slotSize = 1.0f/16.0f;
	if(iconType != Icon::TYPE_TERRAIN)
	{
		texturesByName.insert(stringIconMap::value_type(L"helmetCloth",new SimpleIcon(L"helmetCloth",slotSize*0,slotSize*0,slotSize*(0+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"helmetChain",new SimpleIcon(L"helmetChain",slotSize*1,slotSize*0,slotSize*(1+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"helmetIron",new SimpleIcon(L"helmetIron",slotSize*2,slotSize*0,slotSize*(2+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"helmetDiamond",new SimpleIcon(L"helmetDiamond",slotSize*3,slotSize*0,slotSize*(3+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"helmetGold",new SimpleIcon(L"helmetGold",slotSize*4,slotSize*0,slotSize*(4+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"flintAndSteel",new SimpleIcon(L"flintAndSteel",slotSize*5,slotSize*0,slotSize*(5+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"flint",new SimpleIcon(L"flint",slotSize*6,slotSize*0,slotSize*(6+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"coal",new SimpleIcon(L"coal",slotSize*7,slotSize*0,slotSize*(7+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"string",new SimpleIcon(L"string",slotSize*8,slotSize*0,slotSize*(8+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"seeds",new SimpleIcon(L"seeds",slotSize*9,slotSize*0,slotSize*(9+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"apple",new SimpleIcon(L"apple",slotSize*10,slotSize*0,slotSize*(10+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"appleGold",new SimpleIcon(L"appleGold",slotSize*11,slotSize*0,slotSize*(11+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"egg",new SimpleIcon(L"egg",slotSize*12,slotSize*0,slotSize*(12+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"sugar",new SimpleIcon(L"sugar",slotSize*13,slotSize*0,slotSize*(13+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"snowball",new SimpleIcon(L"snowball",slotSize*14,slotSize*0,slotSize*(14+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"slot_empty_helmet",new SimpleIcon(L"slot_empty_helmet",slotSize*15,slotSize*0,slotSize*(15+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"chestplateCloth",new SimpleIcon(L"chestplateCloth",slotSize*0,slotSize*1,slotSize*(0+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"chestplateChain",new SimpleIcon(L"chestplateChain",slotSize*1,slotSize*1,slotSize*(1+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"chestplateIron",new SimpleIcon(L"chestplateIron",slotSize*2,slotSize*1,slotSize*(2+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"chestplateDiamond",new SimpleIcon(L"chestplateDiamond",slotSize*3,slotSize*1,slotSize*(3+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"chestplateGold",new SimpleIcon(L"chestplateGold",slotSize*4,slotSize*1,slotSize*(4+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"bow",new SimpleIcon(L"bow",slotSize*5,slotSize*1,slotSize*(5+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"brick",new SimpleIcon(L"brick",slotSize*6,slotSize*1,slotSize*(6+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"ingotIron",new SimpleIcon(L"ingotIron",slotSize*7,slotSize*1,slotSize*(7+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"feather",new SimpleIcon(L"feather",slotSize*8,slotSize*1,slotSize*(8+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"wheat",new SimpleIcon(L"wheat",slotSize*9,slotSize*1,slotSize*(9+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"painting",new SimpleIcon(L"painting",slotSize*10,slotSize*1,slotSize*(10+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"reeds",new SimpleIcon(L"reeds",slotSize*11,slotSize*1,slotSize*(11+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"bone",new SimpleIcon(L"bone",slotSize*12,slotSize*1,slotSize*(12+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"cake",new SimpleIcon(L"cake",slotSize*13,slotSize*1,slotSize*(13+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"slimeball",new SimpleIcon(L"slimeball",slotSize*14,slotSize*1,slotSize*(14+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"slot_empty_chestplate",new SimpleIcon(L"slot_empty_chestplate",slotSize*15,slotSize*1,slotSize*(15+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"leggingsCloth",new SimpleIcon(L"leggingsCloth",slotSize*0,slotSize*2,slotSize*(0+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"leggingsChain",new SimpleIcon(L"leggingsChain",slotSize*1,slotSize*2,slotSize*(1+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"leggingsIron",new SimpleIcon(L"leggingsIron",slotSize*2,slotSize*2,slotSize*(2+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"leggingsDiamond",new SimpleIcon(L"leggingsDiamond",slotSize*3,slotSize*2,slotSize*(3+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"leggingsGold",new SimpleIcon(L"leggingsGold",slotSize*4,slotSize*2,slotSize*(4+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"arrow",new SimpleIcon(L"arrow",slotSize*5,slotSize*2,slotSize*(5+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"quiver",new SimpleIcon(L"quiver",slotSize*6,slotSize*2,slotSize*(6+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"ingotGold",new SimpleIcon(L"ingotGold",slotSize*7,slotSize*2,slotSize*(7+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"sulphur",new SimpleIcon(L"sulphur",slotSize*8,slotSize*2,slotSize*(8+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"bread",new SimpleIcon(L"bread",slotSize*9,slotSize*2,slotSize*(9+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"sign",new SimpleIcon(L"sign",slotSize*10,slotSize*2,slotSize*(10+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"doorWood",new SimpleIcon(L"doorWood",slotSize*11,slotSize*2,slotSize*(11+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"doorIron",new SimpleIcon(L"doorIron",slotSize*12,slotSize*2,slotSize*(12+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"bed",new SimpleIcon(L"bed",slotSize*13,slotSize*2,slotSize*(13+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"fireball",new SimpleIcon(L"fireball",slotSize*14,slotSize*2,slotSize*(14+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"slot_empty_leggings",new SimpleIcon(L"slot_empty_leggings",slotSize*15,slotSize*2,slotSize*(15+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"bootsCloth",new SimpleIcon(L"bootsCloth",slotSize*0,slotSize*3,slotSize*(0+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"bootsChain",new SimpleIcon(L"bootsChain",slotSize*1,slotSize*3,slotSize*(1+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"bootsIron",new SimpleIcon(L"bootsIron",slotSize*2,slotSize*3,slotSize*(2+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"bootsDiamond",new SimpleIcon(L"bootsDiamond",slotSize*3,slotSize*3,slotSize*(3+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"bootsGold",new SimpleIcon(L"bootsGold",slotSize*4,slotSize*3,slotSize*(4+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"stick",new SimpleIcon(L"stick",slotSize*5,slotSize*3,slotSize*(5+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"compass",new SimpleIcon(L"compass",slotSize*6,slotSize*3,slotSize*(6+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"compassP0",new SimpleIcon(L"compassP0",slotSize*7,slotSize*14,slotSize*(7+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"compassP1",new SimpleIcon(L"compassP1",slotSize*8,slotSize*14,slotSize*(8+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"compassP2",new SimpleIcon(L"compassP2",slotSize*9,slotSize*14,slotSize*(9+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"compassP3",new SimpleIcon(L"compassP3",slotSize*10,slotSize*14,slotSize*(10+1),slotSize*(14+1))));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"compass",L"compass"));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"compassP0",L"compass"));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"compassP1",L"compass"));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"compassP2",L"compass"));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"compassP3",L"compass"));
		texturesByName.insert(stringIconMap::value_type(L"diamond",new SimpleIcon(L"diamond",slotSize*7,slotSize*3,slotSize*(7+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"redstone",new SimpleIcon(L"redstone",slotSize*8,slotSize*3,slotSize*(8+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"clay",new SimpleIcon(L"clay",slotSize*9,slotSize*3,slotSize*(9+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"paper",new SimpleIcon(L"paper",slotSize*10,slotSize*3,slotSize*(10+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"book",new SimpleIcon(L"book",slotSize*11,slotSize*3,slotSize*(11+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"map",new SimpleIcon(L"map",slotSize*12,slotSize*3,slotSize*(12+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"seeds_pumpkin",new SimpleIcon(L"seeds_pumpkin",slotSize*13,slotSize*3,slotSize*(13+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"seeds_melon",new SimpleIcon(L"seeds_melon",slotSize*14,slotSize*3,slotSize*(14+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"slot_empty_boots",new SimpleIcon(L"slot_empty_boots",slotSize*15,slotSize*3,slotSize*(15+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"swordWood",new SimpleIcon(L"swordWood",slotSize*0,slotSize*4,slotSize*(0+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"swordStone",new SimpleIcon(L"swordStone",slotSize*1,slotSize*4,slotSize*(1+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"swordIron",new SimpleIcon(L"swordIron",slotSize*2,slotSize*4,slotSize*(2+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"swordDiamond",new SimpleIcon(L"swordDiamond",slotSize*3,slotSize*4,slotSize*(3+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"swordGold",new SimpleIcon(L"swordGold",slotSize*4,slotSize*4,slotSize*(4+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"fishingRod",new SimpleIcon(L"fishingRod",slotSize*5,slotSize*4,slotSize*(5+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"clock",new SimpleIcon(L"clock",slotSize*6,slotSize*4,slotSize*(6+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"clockP0",new SimpleIcon(L"clockP0",slotSize*11,slotSize*14,slotSize*(11+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"clockP1",new SimpleIcon(L"clockP1",slotSize*12,slotSize*14,slotSize*(12+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"clockP2",new SimpleIcon(L"clockP2",slotSize*13,slotSize*14,slotSize*(13+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"clockP3",new SimpleIcon(L"clockP3",slotSize*14,slotSize*14,slotSize*(14+1),slotSize*(14+1))));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"clock",L"clock"));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"clockP0",L"clock"));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"clockP1",L"clock"));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"clockP2",L"clock"));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"clockP3",L"clock"));
		texturesByName.insert(stringIconMap::value_type(L"bowl",new SimpleIcon(L"bowl",slotSize*7,slotSize*4,slotSize*(7+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"mushroomStew",new SimpleIcon(L"mushroomStew",slotSize*8,slotSize*4,slotSize*(8+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"yellowDust",new SimpleIcon(L"yellowDust",slotSize*9,slotSize*4,slotSize*(9+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"bucket",new SimpleIcon(L"bucket",slotSize*10,slotSize*4,slotSize*(10+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"bucketWater",new SimpleIcon(L"bucketWater",slotSize*11,slotSize*4,slotSize*(11+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"bucketLava",new SimpleIcon(L"bucketLava",slotSize*12,slotSize*4,slotSize*(12+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"milk",new SimpleIcon(L"milk",slotSize*13,slotSize*4,slotSize*(13+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_black",new SimpleIcon(L"dyePowder_black",slotSize*14,slotSize*4,slotSize*(14+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_gray",new SimpleIcon(L"dyePowder_gray",slotSize*15,slotSize*4,slotSize*(15+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"shovelWood",new SimpleIcon(L"shovelWood",slotSize*0,slotSize*5,slotSize*(0+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"shovelStone",new SimpleIcon(L"shovelStone",slotSize*1,slotSize*5,slotSize*(1+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"shovelIron",new SimpleIcon(L"shovelIron",slotSize*2,slotSize*5,slotSize*(2+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"shovelDiamond",new SimpleIcon(L"shovelDiamond",slotSize*3,slotSize*5,slotSize*(3+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"shovelGold",new SimpleIcon(L"shovelGold",slotSize*4,slotSize*5,slotSize*(4+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"fishingRod_empty",new SimpleIcon(L"fishingRod_empty",slotSize*5,slotSize*5,slotSize*(5+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"diode",new SimpleIcon(L"diode",slotSize*6,slotSize*5,slotSize*(6+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"porkchopRaw",new SimpleIcon(L"porkchopRaw",slotSize*7,slotSize*5,slotSize*(7+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"porkchopCooked",new SimpleIcon(L"porkchopCooked",slotSize*8,slotSize*5,slotSize*(8+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"fishRaw",new SimpleIcon(L"fishRaw",slotSize*9,slotSize*5,slotSize*(9+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"fishCooked",new SimpleIcon(L"fishCooked",slotSize*10,slotSize*5,slotSize*(10+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"rottenFlesh",new SimpleIcon(L"rottenFlesh",slotSize*11,slotSize*5,slotSize*(11+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"cookie",new SimpleIcon(L"cookie",slotSize*12,slotSize*5,slotSize*(12+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"shears",new SimpleIcon(L"shears",slotSize*13,slotSize*5,slotSize*(13+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_red",new SimpleIcon(L"dyePowder_red",slotSize*14,slotSize*5,slotSize*(14+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_pink",new SimpleIcon(L"dyePowder_pink",slotSize*15,slotSize*5,slotSize*(15+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"pickaxeWood",new SimpleIcon(L"pickaxeWood",slotSize*0,slotSize*6,slotSize*(0+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"pickaxeStone",new SimpleIcon(L"pickaxeStone",slotSize*1,slotSize*6,slotSize*(1+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"pickaxeIron",new SimpleIcon(L"pickaxeIron",slotSize*2,slotSize*6,slotSize*(2+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"pickaxeDiamond",new SimpleIcon(L"pickaxeDiamond",slotSize*3,slotSize*6,slotSize*(3+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"pickaxeGold",new SimpleIcon(L"pickaxeGold",slotSize*4,slotSize*6,slotSize*(4+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"bow_pull_0",new SimpleIcon(L"bow_pull_0",slotSize*5,slotSize*6,slotSize*(5+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"carrotOnAStick",new SimpleIcon(L"carrotOnAStick",slotSize*6,slotSize*6,slotSize*(6+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"leather",new SimpleIcon(L"leather",slotSize*7,slotSize*6,slotSize*(7+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"saddle",new SimpleIcon(L"saddle",slotSize*8,slotSize*6,slotSize*(8+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"beefRaw",new SimpleIcon(L"beefRaw",slotSize*9,slotSize*6,slotSize*(9+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"beefCooked",new SimpleIcon(L"beefCooked",slotSize*10,slotSize*6,slotSize*(10+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"enderPearl",new SimpleIcon(L"enderPearl",slotSize*11,slotSize*6,slotSize*(11+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"blazeRod",new SimpleIcon(L"blazeRod",slotSize*12,slotSize*6,slotSize*(12+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"melon",new SimpleIcon(L"melon",slotSize*13,slotSize*6,slotSize*(13+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_green",new SimpleIcon(L"dyePowder_green",slotSize*14,slotSize*6,slotSize*(14+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_lime",new SimpleIcon(L"dyePowder_lime",slotSize*15,slotSize*6,slotSize*(15+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"hatchetWood",new SimpleIcon(L"hatchetWood",slotSize*0,slotSize*7,slotSize*(0+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"hatchetStone",new SimpleIcon(L"hatchetStone",slotSize*1,slotSize*7,slotSize*(1+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"hatchetIron",new SimpleIcon(L"hatchetIron",slotSize*2,slotSize*7,slotSize*(2+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"hatchetDiamond",new SimpleIcon(L"hatchetDiamond",slotSize*3,slotSize*7,slotSize*(3+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"hatchetGold",new SimpleIcon(L"hatchetGold",slotSize*4,slotSize*7,slotSize*(4+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"bow_pull_1",new SimpleIcon(L"bow_pull_1",slotSize*5,slotSize*7,slotSize*(5+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"potatoBaked",new SimpleIcon(L"potatoBaked",slotSize*6,slotSize*7,slotSize*(6+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"potato",new SimpleIcon(L"potato",slotSize*7,slotSize*7,slotSize*(7+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"carrots",new SimpleIcon(L"carrots",slotSize*8,slotSize*7,slotSize*(8+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"chickenRaw",new SimpleIcon(L"chickenRaw",slotSize*9,slotSize*7,slotSize*(9+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"chickenCooked",new SimpleIcon(L"chickenCooked",slotSize*10,slotSize*7,slotSize*(10+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"ghastTear",new SimpleIcon(L"ghastTear",slotSize*11,slotSize*7,slotSize*(11+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"goldNugget",new SimpleIcon(L"goldNugget",slotSize*12,slotSize*7,slotSize*(12+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"netherStalkSeeds",new SimpleIcon(L"netherStalkSeeds",slotSize*13,slotSize*7,slotSize*(13+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_brown",new SimpleIcon(L"dyePowder_brown",slotSize*14,slotSize*7,slotSize*(14+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_yellow",new SimpleIcon(L"dyePowder_yellow",slotSize*15,slotSize*7,slotSize*(15+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"hoeWood",new SimpleIcon(L"hoeWood",slotSize*0,slotSize*8,slotSize*(0+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"hoeStone",new SimpleIcon(L"hoeStone",slotSize*1,slotSize*8,slotSize*(1+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"hoeIron",new SimpleIcon(L"hoeIron",slotSize*2,slotSize*8,slotSize*(2+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"hoeDiamond",new SimpleIcon(L"hoeDiamond",slotSize*3,slotSize*8,slotSize*(3+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"hoeGold",new SimpleIcon(L"hoeGold",slotSize*4,slotSize*8,slotSize*(4+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"bow_pull_2",new SimpleIcon(L"bow_pull_2",slotSize*5,slotSize*8,slotSize*(5+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"potatoPoisonous",new SimpleIcon(L"potatoPoisonous",slotSize*6,slotSize*8,slotSize*(6+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"minecart",new SimpleIcon(L"minecart",slotSize*7,slotSize*8,slotSize*(7+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"boat",new SimpleIcon(L"boat",slotSize*8,slotSize*8,slotSize*(8+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"speckledMelon",new SimpleIcon(L"speckledMelon",slotSize*9,slotSize*8,slotSize*(9+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"fermentedSpiderEye",new SimpleIcon(L"fermentedSpiderEye",slotSize*10,slotSize*8,slotSize*(10+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"spiderEye",new SimpleIcon(L"spiderEye",slotSize*11,slotSize*8,slotSize*(11+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"potion",new SimpleIcon(L"potion",slotSize*12,slotSize*8,slotSize*(12+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"glassBottle",new SimpleIcon(L"glassBottle",slotSize*12,slotSize*8,slotSize*(12+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"potion_contents",new SimpleIcon(L"potion_contents",slotSize*13,slotSize*8,slotSize*(13+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_blue",new SimpleIcon(L"dyePowder_blue",slotSize*14,slotSize*8,slotSize*(14+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_lightBlue",new SimpleIcon(L"dyePowder_lightBlue",slotSize*15,slotSize*8,slotSize*(15+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"helmetCloth_overlay",new SimpleIcon(L"helmetCloth_overlay",slotSize*0,slotSize*9,slotSize*(0+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"comparator",new SimpleIcon(L"comparator",slotSize*5,slotSize*9,slotSize*(5+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"carrotGolden",new SimpleIcon(L"carrotGolden",slotSize*6,slotSize*9,slotSize*(6+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"minecartChest",new SimpleIcon(L"minecartChest",slotSize*7,slotSize*9,slotSize*(7+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"pumpkinPie",new SimpleIcon(L"pumpkinPie",slotSize*8,slotSize*9,slotSize*(8+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"monsterPlacer",new SimpleIcon(L"monsterPlacer",slotSize*9,slotSize*9,slotSize*(9+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"potion_splash",new SimpleIcon(L"potion_splash",slotSize*10,slotSize*9,slotSize*(10+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"eyeOfEnder",new SimpleIcon(L"eyeOfEnder",slotSize*11,slotSize*9,slotSize*(11+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"cauldron",new SimpleIcon(L"cauldron",slotSize*12,slotSize*9,slotSize*(12+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"blazePowder",new SimpleIcon(L"blazePowder",slotSize*13,slotSize*9,slotSize*(13+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_purple",new SimpleIcon(L"dyePowder_purple",slotSize*14,slotSize*9,slotSize*(14+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_magenta",new SimpleIcon(L"dyePowder_magenta",slotSize*15,slotSize*9,slotSize*(15+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"chestplateCloth_overlay",new SimpleIcon(L"chestplateCloth_overlay",slotSize*0,slotSize*10,slotSize*(0+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"netherbrick",new SimpleIcon(L"netherbrick",slotSize*5,slotSize*10,slotSize*(5+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"minecartFurnace",new SimpleIcon(L"minecartFurnace",slotSize*7,slotSize*10,slotSize*(7+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"monsterPlacer_overlay",new SimpleIcon(L"monsterPlacer_overlay",slotSize*9,slotSize*10,slotSize*(9+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"ruby",new SimpleIcon(L"ruby",slotSize*10,slotSize*10,slotSize*(10+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"expBottle",new SimpleIcon(L"expBottle",slotSize*11,slotSize*10,slotSize*(11+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"brewingStand",new SimpleIcon(L"brewingStand",slotSize*12,slotSize*10,slotSize*(12+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"magmaCream",new SimpleIcon(L"magmaCream",slotSize*13,slotSize*10,slotSize*(13+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_cyan",new SimpleIcon(L"dyePowder_cyan",slotSize*14,slotSize*10,slotSize*(14+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_orange",new SimpleIcon(L"dyePowder_orange",slotSize*15,slotSize*10,slotSize*(15+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"leggingsCloth_overlay",new SimpleIcon(L"leggingsCloth_overlay",slotSize*0,slotSize*11,slotSize*(0+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"minecartHopper",new SimpleIcon(L"minecartHopper",slotSize*7,slotSize*11,slotSize*(7+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"hopper",new SimpleIcon(L"hopper",slotSize*8,slotSize*11,slotSize*(8+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"netherStar",new SimpleIcon(L"netherStar",slotSize*9,slotSize*11,slotSize*(9+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"emerald",new SimpleIcon(L"emerald",slotSize*10,slotSize*11,slotSize*(10+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"writingBook",new SimpleIcon(L"writingBook",slotSize*11,slotSize*11,slotSize*(11+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"writtenBook",new SimpleIcon(L"writtenBook",slotSize*12,slotSize*11,slotSize*(12+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"flowerPot",new SimpleIcon(L"flowerPot",slotSize*13,slotSize*11,slotSize*(13+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_silver",new SimpleIcon(L"dyePowder_silver",slotSize*14,slotSize*11,slotSize*(14+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"dyePowder_white",new SimpleIcon(L"dyePowder_white",slotSize*15,slotSize*11,slotSize*(15+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"bootsCloth_overlay",new SimpleIcon(L"bootsCloth_overlay",slotSize*0,slotSize*12,slotSize*(0+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"minecartTnt",new SimpleIcon(L"minecartTnt",slotSize*7,slotSize*12,slotSize*(7+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"fireworks",new SimpleIcon(L"fireworks",slotSize*9,slotSize*12,slotSize*(9+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"fireworksCharge",new SimpleIcon(L"fireworksCharge",slotSize*10,slotSize*12,slotSize*(10+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"fireworksCharge_overlay",new SimpleIcon(L"fireworksCharge_overlay",slotSize*11,slotSize*12,slotSize*(11+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"netherquartz",new SimpleIcon(L"netherquartz",slotSize*12,slotSize*12,slotSize*(12+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"emptyMap",new SimpleIcon(L"emptyMap",slotSize*13,slotSize*12,slotSize*(13+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"frame",new SimpleIcon(L"frame",slotSize*14,slotSize*12,slotSize*(14+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"enchantedBook",new SimpleIcon(L"enchantedBook",slotSize*15,slotSize*12,slotSize*(15+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"skull_skeleton",new SimpleIcon(L"skull_skeleton",slotSize*0,slotSize*14,slotSize*(0+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"skull_wither",new SimpleIcon(L"skull_wither",slotSize*1,slotSize*14,slotSize*(1+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"skull_zombie",new SimpleIcon(L"skull_zombie",slotSize*2,slotSize*14,slotSize*(2+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"skull_char",new SimpleIcon(L"skull_char",slotSize*3,slotSize*14,slotSize*(3+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"skull_creeper",new SimpleIcon(L"skull_creeper",slotSize*4,slotSize*14,slotSize*(4+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"dragonFireball",new SimpleIcon(L"dragonFireball",slotSize*15,slotSize*14,slotSize*(15+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"record_13",new SimpleIcon(L"record_13",slotSize*0,slotSize*15,slotSize*(0+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"record_cat",new SimpleIcon(L"record_cat",slotSize*1,slotSize*15,slotSize*(1+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"record_blocks",new SimpleIcon(L"record_blocks",slotSize*2,slotSize*15,slotSize*(2+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"record_chirp",new SimpleIcon(L"record_chirp",slotSize*3,slotSize*15,slotSize*(3+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"record_far",new SimpleIcon(L"record_far",slotSize*4,slotSize*15,slotSize*(4+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"record_mall",new SimpleIcon(L"record_mall",slotSize*5,slotSize*15,slotSize*(5+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"record_mellohi",new SimpleIcon(L"record_mellohi",slotSize*6,slotSize*15,slotSize*(6+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"record_stal",new SimpleIcon(L"record_stal",slotSize*7,slotSize*15,slotSize*(7+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"record_strad",new SimpleIcon(L"record_strad",slotSize*8,slotSize*15,slotSize*(8+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"record_ward",new SimpleIcon(L"record_ward",slotSize*9,slotSize*15,slotSize*(9+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"record_11",new SimpleIcon(L"record_11",slotSize*10,slotSize*15,slotSize*(10+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"record_where are we now",new SimpleIcon(L"record_where are we now",slotSize*11,slotSize*15,slotSize*(11+1),slotSize*(15+1))));

		// Special cases
		ClockTexture *dataClock = new ClockTexture();
		Icon *oldClock = texturesByName[L"clock"];
		dataClock->initUVs(oldClock->getU0(), oldClock->getV0(), oldClock->getU1(), oldClock->getV1() );
		delete oldClock;
		texturesByName[L"clock"] = dataClock;

		ClockTexture *clock = new ClockTexture(0, dataClock);
		oldClock = texturesByName[L"clockP0"];
		clock->initUVs(oldClock->getU0(), oldClock->getV0(), oldClock->getU1(), oldClock->getV1() );
		delete oldClock;
		texturesByName[L"clockP0"] = clock;

		clock = new ClockTexture(1, dataClock);
		oldClock = texturesByName[L"clockP1"];
		clock->initUVs(oldClock->getU0(), oldClock->getV0(), oldClock->getU1(), oldClock->getV1() );
		delete oldClock;
		texturesByName[L"clockP1"] = clock;

		clock = new ClockTexture(2, dataClock);
		oldClock = texturesByName[L"clockP2"];
		clock->initUVs(oldClock->getU0(), oldClock->getV0(), oldClock->getU1(), oldClock->getV1() );
		delete oldClock;
		texturesByName[L"clockP2"] = clock;

		clock = new ClockTexture(3, dataClock);
		oldClock = texturesByName[L"clockP3"];
		clock->initUVs(oldClock->getU0(), oldClock->getV0(), oldClock->getU1(), oldClock->getV1() );
		delete oldClock;
		texturesByName[L"clockP3"] = clock;

		CompassTexture *dataCompass = new CompassTexture();
		Icon *oldCompass = texturesByName[L"compass"];
		dataCompass->initUVs(oldCompass->getU0(), oldCompass->getV0(), oldCompass->getU1(), oldCompass->getV1() );
		delete oldCompass;
		texturesByName[L"compass"] = dataCompass;

		CompassTexture *compass = new CompassTexture(0, dataCompass);
		oldCompass = texturesByName[L"compassP0"];
		compass->initUVs(oldCompass->getU0(), oldCompass->getV0(), oldCompass->getU1(), oldCompass->getV1() );
		delete oldCompass;
		texturesByName[L"compassP0"] = compass;

		compass = new CompassTexture(1, dataCompass);
		oldCompass = texturesByName[L"compassP1"];
		compass->initUVs(oldCompass->getU0(), oldCompass->getV0(), oldCompass->getU1(), oldCompass->getV1() );
		delete oldCompass;
		texturesByName[L"compassP1"] = compass;

		compass = new CompassTexture(2, dataCompass);
		oldCompass = texturesByName[L"compassP2"];
		compass->initUVs(oldCompass->getU0(), oldCompass->getV0(), oldCompass->getU1(), oldCompass->getV1() );
		delete oldCompass;
		texturesByName[L"compassP2"] = compass;

		compass = new CompassTexture(3, dataCompass);
		oldCompass = texturesByName[L"compassP3"];
		compass->initUVs(oldCompass->getU0(), oldCompass->getV0(), oldCompass->getU1(), oldCompass->getV1() );
		delete oldCompass;
		texturesByName[L"compassP3"] = compass;
	}
	else
	{
		texturesByName.insert(stringIconMap::value_type(L"grass_top",new SimpleIcon(L"grass_top",slotSize*0,slotSize*0,slotSize*(0+1),slotSize*(0+1))));
		texturesByName[L"grass_top"]->setFlags(Icon::IS_GRASS_TOP);			// 4J added for faster determination of texture type in tesselation
		texturesByName.insert(stringIconMap::value_type(L"stone",new SimpleIcon(L"stone",slotSize*1,slotSize*0,slotSize*(1+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"dirt",new SimpleIcon(L"dirt",slotSize*2,slotSize*0,slotSize*(2+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"grass_side",new SimpleIcon(L"grass_side",slotSize*3,slotSize*0,slotSize*(3+1),slotSize*(0+1))));
		texturesByName[L"grass_side"]->setFlags(Icon::IS_GRASS_SIDE);		// 4J added for faster determination of texture type in tesselation
		texturesByName.insert(stringIconMap::value_type(L"wood",new SimpleIcon(L"wood",slotSize*4,slotSize*0,slotSize*(4+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"stoneslab_side",new SimpleIcon(L"stoneslab_side",slotSize*5,slotSize*0,slotSize*(5+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"stoneslab_top",new SimpleIcon(L"stoneslab_top",slotSize*6,slotSize*0,slotSize*(6+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"brick",new SimpleIcon(L"brick",slotSize*7,slotSize*0,slotSize*(7+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"tnt_side",new SimpleIcon(L"tnt_side",slotSize*8,slotSize*0,slotSize*(8+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"tnt_top",new SimpleIcon(L"tnt_top",slotSize*9,slotSize*0,slotSize*(9+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"tnt_bottom",new SimpleIcon(L"tnt_bottom",slotSize*10,slotSize*0,slotSize*(10+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"web",new SimpleIcon(L"web",slotSize*11,slotSize*0,slotSize*(11+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"rose",new SimpleIcon(L"rose",slotSize*12,slotSize*0,slotSize*(12+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"flower",new SimpleIcon(L"flower",slotSize*13,slotSize*0,slotSize*(13+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"portal",new SimpleIcon(L"portal",slotSize*14,slotSize*0,slotSize*(14+1),slotSize*(0+1))));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"portal",L"portal"));
		texturesByName.insert(stringIconMap::value_type(L"sapling",new SimpleIcon(L"sapling",slotSize*15,slotSize*0,slotSize*(15+1),slotSize*(0+1))));
		texturesByName.insert(stringIconMap::value_type(L"stonebrick",new SimpleIcon(L"stonebrick",slotSize*0,slotSize*1,slotSize*(0+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"bedrock",new SimpleIcon(L"bedrock",slotSize*1,slotSize*1,slotSize*(1+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"sand",new SimpleIcon(L"sand",slotSize*2,slotSize*1,slotSize*(2+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"gravel",new SimpleIcon(L"gravel",slotSize*3,slotSize*1,slotSize*(3+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"tree_side",new SimpleIcon(L"tree_side",slotSize*4,slotSize*1,slotSize*(4+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"tree_top",new SimpleIcon(L"tree_top",slotSize*5,slotSize*1,slotSize*(5+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"blockIron",new SimpleIcon(L"blockIron",slotSize*6,slotSize*1,slotSize*(6+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"blockGold",new SimpleIcon(L"blockGold",slotSize*7,slotSize*1,slotSize*(7+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"blockDiamond",new SimpleIcon(L"blockDiamond",slotSize*8,slotSize*1,slotSize*(8+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"blockEmerald",new SimpleIcon(L"blockEmerald",slotSize*9,slotSize*1,slotSize*(9+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"blockRedstone",new SimpleIcon(L"blockRedstone",slotSize*10,slotSize*1,slotSize*(10+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"dropper_front",new SimpleIcon(L"dropper_front",slotSize*11,slotSize*1,slotSize*(11+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"mushroom_red",new SimpleIcon(L"mushroom_red",slotSize*12,slotSize*1,slotSize*(12+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"mushroom_brown",new SimpleIcon(L"mushroom_brown",slotSize*13,slotSize*1,slotSize*(13+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"sapling_jungle",new SimpleIcon(L"sapling_jungle",slotSize*14,slotSize*1,slotSize*(14+1),slotSize*(1+1))));
		texturesByName.insert(stringIconMap::value_type(L"fire_0",new SimpleIcon(L"fire_0",slotSize*15,slotSize*1,slotSize*(15+1),slotSize*(1+1))));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"fire_0",L"fire_0"));
		texturesByName.insert(stringIconMap::value_type(L"oreGold",new SimpleIcon(L"oreGold",slotSize*0,slotSize*2,slotSize*(0+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"oreIron",new SimpleIcon(L"oreIron",slotSize*1,slotSize*2,slotSize*(1+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"oreCoal",new SimpleIcon(L"oreCoal",slotSize*2,slotSize*2,slotSize*(2+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"bookshelf",new SimpleIcon(L"bookshelf",slotSize*3,slotSize*2,slotSize*(3+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"stoneMoss",new SimpleIcon(L"stoneMoss",slotSize*4,slotSize*2,slotSize*(4+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"obsidian",new SimpleIcon(L"obsidian",slotSize*5,slotSize*2,slotSize*(5+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"grass_side_overlay",new SimpleIcon(L"grass_side_overlay",slotSize*6,slotSize*2,slotSize*(6+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"tallgrass",new SimpleIcon(L"tallgrass",slotSize*7,slotSize*2,slotSize*(7+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"dispenser_front_vertical",new SimpleIcon(L"dispenser_front_vertical",slotSize*8,slotSize*2,slotSize*(8+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"beacon",new SimpleIcon(L"beacon",slotSize*9,slotSize*2,slotSize*(9+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"dropper_front_vertical",new SimpleIcon(L"dropper_front_vertical",slotSize*10,slotSize*2,slotSize*(10+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"workbench_top",new SimpleIcon(L"workbench_top",slotSize*11,slotSize*2,slotSize*(11+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"furnace_front",new SimpleIcon(L"furnace_front",slotSize*12,slotSize*2,slotSize*(12+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"furnace_side",new SimpleIcon(L"furnace_side",slotSize*13,slotSize*2,slotSize*(13+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"dispenser_front",new SimpleIcon(L"dispenser_front",slotSize*14,slotSize*2,slotSize*(14+1),slotSize*(2+1))));
		texturesByName.insert(stringIconMap::value_type(L"fire_1",new SimpleIcon(L"fire_1",slotSize*15,slotSize*1,slotSize*(15+1),slotSize*(1+1))));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"fire_1",L"fire_1"));
		texturesByName.insert(stringIconMap::value_type(L"sponge",new SimpleIcon(L"sponge",slotSize*0,slotSize*3,slotSize*(0+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"glass",new SimpleIcon(L"glass",slotSize*1,slotSize*3,slotSize*(1+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"oreDiamond",new SimpleIcon(L"oreDiamond",slotSize*2,slotSize*3,slotSize*(2+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"oreRedstone",new SimpleIcon(L"oreRedstone",slotSize*3,slotSize*3,slotSize*(3+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"leaves",new SimpleIcon(L"leaves",slotSize*4,slotSize*3,slotSize*(4+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"leaves_opaque",new SimpleIcon(L"leaves_opaque",slotSize*5,slotSize*3,slotSize*(5+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"stonebricksmooth",new SimpleIcon(L"stonebricksmooth",slotSize*6,slotSize*3,slotSize*(6+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"deadbush",new SimpleIcon(L"deadbush",slotSize*7,slotSize*3,slotSize*(7+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"fern",new SimpleIcon(L"fern",slotSize*8,slotSize*3,slotSize*(8+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"daylightDetector_top",new SimpleIcon(L"daylightDetector_top",slotSize*9,slotSize*3,slotSize*(9+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"daylightDetector_side",new SimpleIcon(L"daylightDetector_side",slotSize*10,slotSize*3,slotSize*(10+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"workbench_side",new SimpleIcon(L"workbench_side",slotSize*11,slotSize*3,slotSize*(11+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"workbench_front",new SimpleIcon(L"workbench_front",slotSize*12,slotSize*3,slotSize*(12+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"furnace_front_lit",new SimpleIcon(L"furnace_front_lit",slotSize*13,slotSize*3,slotSize*(13+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"furnace_top",new SimpleIcon(L"furnace_top",slotSize*14,slotSize*3,slotSize*(14+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"sapling_spruce",new SimpleIcon(L"sapling_spruce",slotSize*15,slotSize*3,slotSize*(15+1),slotSize*(3+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_0",new SimpleIcon(L"cloth_0",slotSize*0,slotSize*4,slotSize*(0+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"mobSpawner",new SimpleIcon(L"mobSpawner",slotSize*1,slotSize*4,slotSize*(1+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"snow",new SimpleIcon(L"snow",slotSize*2,slotSize*4,slotSize*(2+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"ice",new SimpleIcon(L"ice",slotSize*3,slotSize*4,slotSize*(3+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"snow_side",new SimpleIcon(L"snow_side",slotSize*4,slotSize*4,slotSize*(4+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"cactus_top",new SimpleIcon(L"cactus_top",slotSize*5,slotSize*4,slotSize*(5+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"cactus_side",new SimpleIcon(L"cactus_side",slotSize*6,slotSize*4,slotSize*(6+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"cactus_bottom",new SimpleIcon(L"cactus_bottom",slotSize*7,slotSize*4,slotSize*(7+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"clay",new SimpleIcon(L"clay",slotSize*8,slotSize*4,slotSize*(8+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"reeds",new SimpleIcon(L"reeds",slotSize*9,slotSize*4,slotSize*(9+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"musicBlock",new SimpleIcon(L"musicBlock",slotSize*10,slotSize*4,slotSize*(10+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"jukebox_top",new SimpleIcon(L"jukebox_top",slotSize*11,slotSize*4,slotSize*(11+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"waterlily",new SimpleIcon(L"waterlily",slotSize*12,slotSize*4,slotSize*(12+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"mycel_side",new SimpleIcon(L"mycel_side",slotSize*13,slotSize*4,slotSize*(13+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"mycel_top",new SimpleIcon(L"mycel_top",slotSize*14,slotSize*4,slotSize*(14+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"sapling_birch",new SimpleIcon(L"sapling_birch",slotSize*15,slotSize*4,slotSize*(15+1),slotSize*(4+1))));
		texturesByName.insert(stringIconMap::value_type(L"torch",new SimpleIcon(L"torch",slotSize*0,slotSize*5,slotSize*(0+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"doorWood_upper",new SimpleIcon(L"doorWood_upper",slotSize*1,slotSize*5,slotSize*(1+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"doorIron_upper",new SimpleIcon(L"doorIron_upper",slotSize*2,slotSize*5,slotSize*(2+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"ladder",new SimpleIcon(L"ladder",slotSize*3,slotSize*5,slotSize*(3+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"trapdoor",new SimpleIcon(L"trapdoor",slotSize*4,slotSize*5,slotSize*(4+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"fenceIron",new SimpleIcon(L"fenceIron",slotSize*5,slotSize*5,slotSize*(5+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"farmland_wet",new SimpleIcon(L"farmland_wet",slotSize*6,slotSize*5,slotSize*(6+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"farmland_dry",new SimpleIcon(L"farmland_dry",slotSize*7,slotSize*5,slotSize*(7+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"crops_0",new SimpleIcon(L"crops_0",slotSize*8,slotSize*5,slotSize*(8+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"crops_1",new SimpleIcon(L"crops_1",slotSize*9,slotSize*5,slotSize*(9+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"crops_2",new SimpleIcon(L"crops_2",slotSize*10,slotSize*5,slotSize*(10+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"crops_3",new SimpleIcon(L"crops_3",slotSize*11,slotSize*5,slotSize*(11+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"crops_4",new SimpleIcon(L"crops_4",slotSize*12,slotSize*5,slotSize*(12+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"crops_5",new SimpleIcon(L"crops_5",slotSize*13,slotSize*5,slotSize*(13+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"crops_6",new SimpleIcon(L"crops_6",slotSize*14,slotSize*5,slotSize*(14+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"crops_7",new SimpleIcon(L"crops_7",slotSize*15,slotSize*5,slotSize*(15+1),slotSize*(5+1))));
		texturesByName.insert(stringIconMap::value_type(L"lever",new SimpleIcon(L"lever",slotSize*0,slotSize*6,slotSize*(0+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"doorWood_lower",new SimpleIcon(L"doorWood_lower",slotSize*1,slotSize*6,slotSize*(1+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"doorIron_lower",new SimpleIcon(L"doorIron_lower",slotSize*2,slotSize*6,slotSize*(2+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"redtorch_lit",new SimpleIcon(L"redtorch_lit",slotSize*3,slotSize*6,slotSize*(3+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"stonebricksmooth_mossy",new SimpleIcon(L"stonebricksmooth_mossy",slotSize*4,slotSize*6,slotSize*(4+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"stonebricksmooth_cracked",new SimpleIcon(L"stonebricksmooth_cracked",slotSize*5,slotSize*6,slotSize*(5+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"pumpkin_top",new SimpleIcon(L"pumpkin_top",slotSize*6,slotSize*6,slotSize*(6+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"hellrock",new SimpleIcon(L"hellrock",slotSize*7,slotSize*6,slotSize*(7+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"hellsand",new SimpleIcon(L"hellsand",slotSize*8,slotSize*6,slotSize*(8+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"lightgem",new SimpleIcon(L"lightgem",slotSize*9,slotSize*6,slotSize*(9+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"piston_top_sticky",new SimpleIcon(L"piston_top_sticky",slotSize*10,slotSize*6,slotSize*(10+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"piston_top",new SimpleIcon(L"piston_top",slotSize*11,slotSize*6,slotSize*(11+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"piston_side",new SimpleIcon(L"piston_side",slotSize*12,slotSize*6,slotSize*(12+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"piston_bottom",new SimpleIcon(L"piston_bottom",slotSize*13,slotSize*6,slotSize*(13+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"piston_inner_top",new SimpleIcon(L"piston_inner_top",slotSize*14,slotSize*6,slotSize*(14+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"stem_straight",new SimpleIcon(L"stem_straight",slotSize*15,slotSize*6,slotSize*(15+1),slotSize*(6+1))));
		texturesByName.insert(stringIconMap::value_type(L"rail_turn",new SimpleIcon(L"rail_turn",slotSize*0,slotSize*7,slotSize*(0+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_15",new SimpleIcon(L"cloth_15",slotSize*1,slotSize*7,slotSize*(1+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_7",new SimpleIcon(L"cloth_7",slotSize*2,slotSize*7,slotSize*(2+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"redtorch",new SimpleIcon(L"redtorch",slotSize*3,slotSize*7,slotSize*(3+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"tree_spruce",new SimpleIcon(L"tree_spruce",slotSize*4,slotSize*7,slotSize*(4+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"tree_birch",new SimpleIcon(L"tree_birch",slotSize*5,slotSize*7,slotSize*(5+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"pumpkin_side",new SimpleIcon(L"pumpkin_side",slotSize*6,slotSize*7,slotSize*(6+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"pumpkin_face",new SimpleIcon(L"pumpkin_face",slotSize*7,slotSize*7,slotSize*(7+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"pumpkin_jack",new SimpleIcon(L"pumpkin_jack",slotSize*8,slotSize*7,slotSize*(8+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"cake_top",new SimpleIcon(L"cake_top",slotSize*9,slotSize*7,slotSize*(9+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"cake_side",new SimpleIcon(L"cake_side",slotSize*10,slotSize*7,slotSize*(10+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"cake_inner",new SimpleIcon(L"cake_inner",slotSize*11,slotSize*7,slotSize*(11+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"cake_bottom",new SimpleIcon(L"cake_bottom",slotSize*12,slotSize*7,slotSize*(12+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"mushroom_skin_red",new SimpleIcon(L"mushroom_skin_red",slotSize*13,slotSize*7,slotSize*(13+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"mushroom_skin_brown",new SimpleIcon(L"mushroom_skin_brown",slotSize*14,slotSize*7,slotSize*(14+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"stem_bent",new SimpleIcon(L"stem_bent",slotSize*15,slotSize*7,slotSize*(15+1),slotSize*(7+1))));
		texturesByName.insert(stringIconMap::value_type(L"rail",new SimpleIcon(L"rail",slotSize*0,slotSize*8,slotSize*(0+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_14",new SimpleIcon(L"cloth_14",slotSize*1,slotSize*8,slotSize*(1+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_6",new SimpleIcon(L"cloth_6",slotSize*2,slotSize*8,slotSize*(2+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"repeater",new SimpleIcon(L"repeater",slotSize*3,slotSize*8,slotSize*(3+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"leaves_spruce",new SimpleIcon(L"leaves_spruce",slotSize*4,slotSize*8,slotSize*(4+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"leaves_spruce_opaque",new SimpleIcon(L"leaves_spruce_opaque",slotSize*5,slotSize*8,slotSize*(5+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"bed_feet_top",new SimpleIcon(L"bed_feet_top",slotSize*6,slotSize*8,slotSize*(6+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"bed_head_top",new SimpleIcon(L"bed_head_top",slotSize*7,slotSize*8,slotSize*(7+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"melon_side",new SimpleIcon(L"melon_side",slotSize*8,slotSize*8,slotSize*(8+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"melon_top",new SimpleIcon(L"melon_top",slotSize*9,slotSize*8,slotSize*(9+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"cauldron_top",new SimpleIcon(L"cauldron_top",slotSize*10,slotSize*8,slotSize*(10+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"cauldron_inner",new SimpleIcon(L"cauldron_inner",slotSize*11,slotSize*8,slotSize*(11+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"mushroom_skin_stem",new SimpleIcon(L"mushroom_skin_stem",slotSize*13,slotSize*8,slotSize*(13+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"mushroom_inside",new SimpleIcon(L"mushroom_inside",slotSize*14,slotSize*8,slotSize*(14+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"vine",new SimpleIcon(L"vine",slotSize*15,slotSize*8,slotSize*(15+1),slotSize*(8+1))));
		texturesByName.insert(stringIconMap::value_type(L"blockLapis",new SimpleIcon(L"blockLapis",slotSize*0,slotSize*9,slotSize*(0+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_13",new SimpleIcon(L"cloth_13",slotSize*1,slotSize*9,slotSize*(1+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_5",new SimpleIcon(L"cloth_5",slotSize*2,slotSize*9,slotSize*(2+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"repeater_lit",new SimpleIcon(L"repeater_lit",slotSize*3,slotSize*9,slotSize*(3+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"thinglass_top",new SimpleIcon(L"thinglass_top",slotSize*4,slotSize*9,slotSize*(4+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"bed_feet_end",new SimpleIcon(L"bed_feet_end",slotSize*5,slotSize*9,slotSize*(5+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"bed_feet_side",new SimpleIcon(L"bed_feet_side",slotSize*6,slotSize*9,slotSize*(6+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"bed_head_side",new SimpleIcon(L"bed_head_side",slotSize*7,slotSize*9,slotSize*(7+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"bed_head_end",new SimpleIcon(L"bed_head_end",slotSize*8,slotSize*9,slotSize*(8+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"tree_jungle",new SimpleIcon(L"tree_jungle",slotSize*9,slotSize*9,slotSize*(9+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"cauldron_side",new SimpleIcon(L"cauldron_side",slotSize*10,slotSize*9,slotSize*(10+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"cauldron_bottom",new SimpleIcon(L"cauldron_bottom",slotSize*11,slotSize*9,slotSize*(11+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"brewingStand_base",new SimpleIcon(L"brewingStand_base",slotSize*12,slotSize*9,slotSize*(12+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"brewingStand",new SimpleIcon(L"brewingStand",slotSize*13,slotSize*9,slotSize*(13+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"endframe_top",new SimpleIcon(L"endframe_top",slotSize*14,slotSize*9,slotSize*(14+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"endframe_side",new SimpleIcon(L"endframe_side",slotSize*15,slotSize*9,slotSize*(15+1),slotSize*(9+1))));
		texturesByName.insert(stringIconMap::value_type(L"oreLapis",new SimpleIcon(L"oreLapis",slotSize*0,slotSize*10,slotSize*(0+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_12",new SimpleIcon(L"cloth_12",slotSize*1,slotSize*10,slotSize*(1+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_4",new SimpleIcon(L"cloth_4",slotSize*2,slotSize*10,slotSize*(2+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"goldenRail",new SimpleIcon(L"goldenRail",slotSize*3,slotSize*10,slotSize*(3+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"redstoneDust_cross",new SimpleIcon(L"redstoneDust_cross",slotSize*4,slotSize*10,slotSize*(4+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"redstoneDust_line",new SimpleIcon(L"redstoneDust_line",slotSize*5,slotSize*10,slotSize*(5+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"enchantment_top",new SimpleIcon(L"enchantment_top",slotSize*6,slotSize*10,slotSize*(6+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"dragonEgg",new SimpleIcon(L"dragonEgg",slotSize*7,slotSize*10,slotSize*(7+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"cocoa_2",new SimpleIcon(L"cocoa_2",slotSize*8,slotSize*10,slotSize*(8+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"cocoa_1",new SimpleIcon(L"cocoa_1",slotSize*9,slotSize*10,slotSize*(9+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"cocoa_0",new SimpleIcon(L"cocoa_0",slotSize*10,slotSize*10,slotSize*(10+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"oreEmerald",new SimpleIcon(L"oreEmerald",slotSize*11,slotSize*10,slotSize*(11+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"tripWireSource",new SimpleIcon(L"tripWireSource",slotSize*12,slotSize*10,slotSize*(12+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"tripWire",new SimpleIcon(L"tripWire",slotSize*13,slotSize*10,slotSize*(13+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"endframe_eye",new SimpleIcon(L"endframe_eye",slotSize*14,slotSize*10,slotSize*(14+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"whiteStone",new SimpleIcon(L"whiteStone",slotSize*15,slotSize*10,slotSize*(15+1),slotSize*(10+1))));
		texturesByName.insert(stringIconMap::value_type(L"sandstone_top",new SimpleIcon(L"sandstone_top",slotSize*0,slotSize*11,slotSize*(0+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_11",new SimpleIcon(L"cloth_11",slotSize*1,slotSize*11,slotSize*(1+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_3",new SimpleIcon(L"cloth_3",slotSize*2,slotSize*11,slotSize*(2+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"goldenRail_powered",new SimpleIcon(L"goldenRail_powered",slotSize*3,slotSize*11,slotSize*(3+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"redstoneDust_cross_overlay",new SimpleIcon(L"redstoneDust_cross_overlay",slotSize*4,slotSize*11,slotSize*(4+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"redstoneDust_line_overlay",new SimpleIcon(L"redstoneDust_line_overlay",slotSize*5,slotSize*11,slotSize*(5+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"enchantment_side",new SimpleIcon(L"enchantment_side",slotSize*6,slotSize*11,slotSize*(6+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"enchantment_bottom",new SimpleIcon(L"enchantment_bottom",slotSize*7,slotSize*11,slotSize*(7+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"commandBlock",new SimpleIcon(L"commandBlock",slotSize*8,slotSize*11,slotSize*(8+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"itemframe_back",new SimpleIcon(L"itemframe_back",slotSize*9,slotSize*11,slotSize*(9+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"flowerPot",new SimpleIcon(L"flowerPot",slotSize*10,slotSize*11,slotSize*(10+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"comparator",new SimpleIcon(L"comparator",slotSize*11,slotSize*11,slotSize*(11+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"comparator_lit",new SimpleIcon(L"comparator_lit",slotSize*12,slotSize*11,slotSize*(12+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"activatorRail",new SimpleIcon(L"activatorRail",slotSize*13,slotSize*11,slotSize*(13+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"activatorRail_powered",new SimpleIcon(L"activatorRail_powered",slotSize*14,slotSize*11,slotSize*(14+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"netherquartz",new SimpleIcon(L"netherquartz",slotSize*15,slotSize*11,slotSize*(15+1),slotSize*(11+1))));
		texturesByName.insert(stringIconMap::value_type(L"sandstone_side",new SimpleIcon(L"sandstone_side",slotSize*0,slotSize*12,slotSize*(0+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_10",new SimpleIcon(L"cloth_10",slotSize*1,slotSize*12,slotSize*(1+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_2",new SimpleIcon(L"cloth_2",slotSize*2,slotSize*12,slotSize*(2+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"detectorRail",new SimpleIcon(L"detectorRail",slotSize*3,slotSize*12,slotSize*(3+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"leaves_jungle",new SimpleIcon(L"leaves_jungle",slotSize*4,slotSize*12,slotSize*(4+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"leaves_jungle_opaque",new SimpleIcon(L"leaves_jungle_opaque",slotSize*5,slotSize*12,slotSize*(5+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"wood_spruce",new SimpleIcon(L"wood_spruce",slotSize*6,slotSize*12,slotSize*(6+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"wood_jungle",new SimpleIcon(L"wood_jungle",slotSize*7,slotSize*12,slotSize*(7+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"carrots_0",new SimpleIcon(L"carrots_0",slotSize*8,slotSize*12,slotSize*(8+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"carrots_1",new SimpleIcon(L"carrots_1",slotSize*9,slotSize*12,slotSize*(9+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"carrots_2",new SimpleIcon(L"carrots_2",slotSize*10,slotSize*12,slotSize*(10+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"carrots_3",new SimpleIcon(L"carrots_3",slotSize*11,slotSize*12,slotSize*(11+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"potatoes_0",new SimpleIcon(L"potatoes_0",slotSize*8,slotSize*12,slotSize*(8+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"potatoes_1",new SimpleIcon(L"potatoes_1",slotSize*9,slotSize*12,slotSize*(9+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"potatoes_2",new SimpleIcon(L"potatoes_2",slotSize*10,slotSize*12,slotSize*(10+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"potatoes_3",new SimpleIcon(L"potatoes_3",slotSize*12,slotSize*12,slotSize*(12+1),slotSize*(12+1))));
		texturesByName.insert(stringIconMap::value_type(L"water",new SimpleIcon(L"water",slotSize*13,slotSize*12,slotSize*(13+1),slotSize*(12+1))));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"water",L"water"));
		texturesByName.insert(stringIconMap::value_type(L"water_flow",new SimpleIcon(L"water_flow",slotSize*14,slotSize*12,slotSize*(14+2),slotSize*(12+2))));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"water_flow",L"water_flow"));
		texturesByName.insert(stringIconMap::value_type(L"sandstone_bottom",new SimpleIcon(L"sandstone_bottom",slotSize*0,slotSize*13,slotSize*(0+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_9",new SimpleIcon(L"cloth_9",slotSize*1,slotSize*13,slotSize*(1+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_1",new SimpleIcon(L"cloth_1",slotSize*2,slotSize*13,slotSize*(2+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"redstoneLight",new SimpleIcon(L"redstoneLight",slotSize*3,slotSize*13,slotSize*(3+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"redstoneLight_lit",new SimpleIcon(L"redstoneLight_lit",slotSize*4,slotSize*13,slotSize*(4+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"stonebricksmooth_carved",new SimpleIcon(L"stonebricksmooth_carved",slotSize*5,slotSize*13,slotSize*(5+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"wood_birch",new SimpleIcon(L"wood_birch",slotSize*6,slotSize*13,slotSize*(6+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"anvil_base",new SimpleIcon(L"anvil_base",slotSize*7,slotSize*13,slotSize*(7+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"anvil_top_damaged_1",new SimpleIcon(L"anvil_top_damaged_1",slotSize*8,slotSize*13,slotSize*(8+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"quartzblock_chiseled_top",new SimpleIcon(L"quartzblock_chiseled_top",slotSize*9,slotSize*13,slotSize*(9+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"quartzblock_lines_top",new SimpleIcon(L"quartzblock_lines_top",slotSize*10,slotSize*13,slotSize*(10+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"quartzblock_top",new SimpleIcon(L"quartzblock_top",slotSize*11,slotSize*13,slotSize*(11+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"hopper",new SimpleIcon(L"hopper",slotSize*12,slotSize*13,slotSize*(12+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"detectorRail_on",new SimpleIcon(L"detectorRail_on",slotSize*13,slotSize*13,slotSize*(13+1),slotSize*(13+1))));
		texturesByName.insert(stringIconMap::value_type(L"netherBrick",new SimpleIcon(L"netherBrick",slotSize*0,slotSize*14,slotSize*(0+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"cloth_8",new SimpleIcon(L"cloth_8",slotSize*1,slotSize*14,slotSize*(1+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"netherStalk_0",new SimpleIcon(L"netherStalk_0",slotSize*2,slotSize*14,slotSize*(2+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"netherStalk_1",new SimpleIcon(L"netherStalk_1",slotSize*3,slotSize*14,slotSize*(3+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"netherStalk_2",new SimpleIcon(L"netherStalk_2",slotSize*4,slotSize*14,slotSize*(4+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"sandstone_carved",new SimpleIcon(L"sandstone_carved",slotSize*5,slotSize*14,slotSize*(5+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"sandstone_smooth",new SimpleIcon(L"sandstone_smooth",slotSize*6,slotSize*14,slotSize*(6+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"anvil_top",new SimpleIcon(L"anvil_top",slotSize*7,slotSize*14,slotSize*(7+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"anvil_top_damaged_2",new SimpleIcon(L"anvil_top_damaged_2",slotSize*8,slotSize*14,slotSize*(8+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"quartzblock_chiseled",new SimpleIcon(L"quartzblock_chiseled",slotSize*9,slotSize*14,slotSize*(9+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"quartzblock_lines",new SimpleIcon(L"quartzblock_lines",slotSize*10,slotSize*14,slotSize*(10+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"quartzblock_side",new SimpleIcon(L"quartzblock_side",slotSize*11,slotSize*14,slotSize*(11+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"hopper_inside",new SimpleIcon(L"hopper_inside",slotSize*12,slotSize*14,slotSize*(12+1),slotSize*(14+1))));
		texturesByName.insert(stringIconMap::value_type(L"lava",new SimpleIcon(L"lava",slotSize*13,slotSize*14,slotSize*(13+1),slotSize*(14+1))));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"lava",L"lava"));
		texturesByName.insert(stringIconMap::value_type(L"lava_flow",new SimpleIcon(L"lava_flow",slotSize*14,slotSize*14,slotSize*(14+2),slotSize*(14+2))));
		texturesToAnimate.push_back(pair<wstring, wstring>(L"lava_flow",L"lava_flow"));
		texturesByName.insert(stringIconMap::value_type(L"destroy_0",new SimpleIcon(L"destroy_0",slotSize*0,slotSize*15,slotSize*(0+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"destroy_1",new SimpleIcon(L"destroy_1",slotSize*1,slotSize*15,slotSize*(1+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"destroy_2",new SimpleIcon(L"destroy_2",slotSize*2,slotSize*15,slotSize*(2+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"destroy_3",new SimpleIcon(L"destroy_3",slotSize*3,slotSize*15,slotSize*(3+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"destroy_4",new SimpleIcon(L"destroy_4",slotSize*4,slotSize*15,slotSize*(4+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"destroy_5",new SimpleIcon(L"destroy_5",slotSize*5,slotSize*15,slotSize*(5+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"destroy_6",new SimpleIcon(L"destroy_6",slotSize*6,slotSize*15,slotSize*(6+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"destroy_7",new SimpleIcon(L"destroy_7",slotSize*7,slotSize*15,slotSize*(7+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"destroy_8",new SimpleIcon(L"destroy_8",slotSize*8,slotSize*15,slotSize*(8+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"destroy_9",new SimpleIcon(L"destroy_9",slotSize*9,slotSize*15,slotSize*(9+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"quartzblock_bottom",new SimpleIcon(L"quartzblock_bottom",slotSize*11,slotSize*15,slotSize*(11+1),slotSize*(15+1))));
		texturesByName.insert(stringIconMap::value_type(L"hopper_top",new SimpleIcon(L"hopper_top",slotSize*12,slotSize*15,slotSize*(12+1),slotSize*(15+1))));
	}
}
