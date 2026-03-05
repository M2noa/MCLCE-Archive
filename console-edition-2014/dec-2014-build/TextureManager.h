#pragma once

class Texture;
class Stitcher;
class TexturePack;

class TextureManager
{
private:
	static TextureManager *instance;

	int nextID;
	typedef unordered_map<wstring, int> stringIntMap;
	typedef unordered_map<int, Texture *> intTextureMap;
	intTextureMap idToTextureMap;
	stringIntMap stringToIDMap;

public:
	static void createInstance();
	static TextureManager *getInstance();

private:
	TextureManager();

public:
	int createTextureID();
	Texture *getTexture(const wstring &name);
	void registerName(const wstring &name, Texture *texture);
	void registerTexture(Texture *texture);
	void unregisterTexture(const wstring &name, Texture *texture);
	Stitcher *createStitcher(const wstring &name);
	vector<Texture *> *createTextures(const wstring &filename, bool mipmap); // 4J added mipmap param

private:
	wstring getTextureNameFromPath(const wstring &filename);
	bool isAnimation(const wstring &filename, TexturePack *texturePack);

public:
	Texture *createTexture(const wstring &name, int mode, int width, int height, int wrap, int format, int minFilter, int magFilter, bool mipmap, BufferedImage *image);
	Texture *createTexture(const wstring &name, int mode, int width, int height, int format, bool mipmap); // 4J Added mipmap param
};