#pragma once
using namespace std;

class StitchSlot;
class Texture;
#include "TextureHolder.h"

class Stitcher
{
public:
	static const int STITCH_SUCCESS = 0;
	static const int STITCH_RETRY = 1;
	static const int STITCH_ABORT = 2;

	static const int MAX_MIPLEVEL = 0; // This should be 4 again later when we *ACTUALLY* mipmap
	static const int MIN_TEXEL = 1 << MAX_MIPLEVEL;

private:
	set<TextureHolder *, TextureHolderLessThan> texturesToBeStitched; // = new HashSet<TextureHolder>(256);
	vector<StitchSlot *> storage; // = new ArrayList<StitchSlot>(256);
	int storageX;
	int storageY;

	int maxWidth;
	int maxHeight;
	bool forcePowerOfTwo;
	int forcedScale;

	Texture *stitchedTexture;

	wstring name;

	void _init(const wstring &name, int maxWidth, int maxHeight, bool forcePowerOfTwo, int forcedScale);

public:
	Stitcher(const wstring &name, int maxWidth, int maxHeight, bool forcePowerOfTwo);
	Stitcher(const wstring &name, int maxWidth, int maxHeight, bool forcePowerOfTwo, int forcedScale);

	int getWidth();
	int getHeight();
	void addTexture(TextureHolder *textureHolder);
	Texture *constructTexture(bool mipmap = true); // 4J Added mipmap param
	void stitch();
	vector<StitchSlot *> *gatherAreas();

private:	
	// Based on: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
	int smallestEncompassingPowerOfTwo(int input);

	bool addToStorage(TextureHolder *textureHolder);

	/**
	* Expand the current storage to take in account the new texture.
	* This should only be called if it didn't fit anywhere.
	*
	* @param textureHolder
	* @return Boolean indicating if it could accommodate for the growth
	*/
	bool expand(TextureHolder *textureHolder);
};