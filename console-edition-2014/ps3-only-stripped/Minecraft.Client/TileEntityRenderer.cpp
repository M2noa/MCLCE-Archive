#include "stdafx.h"
#include "TileEntityRenderer.h"
#include "TileEntityRenderDispatcher.h"

void TileEntityRenderer::bindTexture(int resourceName)
{
    Textures *t = tileEntityRenderDispatcher->textures;
    if(t != NULL) t->bind(t->loadTexture(resourceName));
}

void TileEntityRenderer::bindTexture(const wstring& urlTexture, int backupTexture)
{
    Textures *t = tileEntityRenderDispatcher->textures;
    if(t != NULL) t->bind(t->loadHttpTexture(urlTexture, backupTexture));
}

Level *TileEntityRenderer::getLevel()
{
	return tileEntityRenderDispatcher->level;
}

void TileEntityRenderer::init(TileEntityRenderDispatcher *tileEntityRenderDispatcher)
{
	this->tileEntityRenderDispatcher = tileEntityRenderDispatcher;
}

Font *TileEntityRenderer::getFont()
{
	return tileEntityRenderDispatcher->getFont();
}