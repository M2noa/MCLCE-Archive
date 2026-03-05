#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "StructureStart.h"
#include "StructurePiece.h"
#include "BoundingBox.h"


StructureStart::StructureStart()
{
	boundingBox = NULL;		// 4J added initialiser
}

StructureStart::~StructureStart()
{
	for(AUTO_VAR(it, pieces.begin()); it != pieces.end(); it++ )
	{
		delete (*it);
	}
	delete boundingBox;
}

BoundingBox *StructureStart::getBoundingBox()
{
	return boundingBox;
}

list<StructurePiece *> *StructureStart::getPieces()
{
	return &pieces;
}

void StructureStart::postProcess(Level *level, Random *random, BoundingBox *chunkBB)
{
	AUTO_VAR(it, pieces.begin());

	while( it != pieces.end() )
	{
		if( (*it)->getBoundingBox()->intersects(chunkBB) && !(*it)->postProcess(level, random, chunkBB))
		{
            // this piece can't be placed, so remove it to avoid future
            // attempts
			it = pieces.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void StructureStart::calculateBoundingBox()
{
    boundingBox = BoundingBox::getUnknownBox();

	for( AUTO_VAR(it, pieces.begin()); it != pieces.end(); it++ )
	{
		StructurePiece *piece = *it;
		boundingBox->expand(piece->getBoundingBox());
	}
}

void StructureStart::moveBelowSeaLevel(Level *level, Random *random, int offset)
{
	const int MAX_Y = level->seaLevel - offset;

    // set lowest possible position (at bedrock)
    int y1Pos = boundingBox->getYSpan() + 1;
    // move up randomly within the available span
    if (y1Pos < MAX_Y)
	{
        y1Pos += random->nextInt(MAX_Y - y1Pos);
    }

    // move all bounding boxes
    int dy = y1Pos - boundingBox->y1;
    boundingBox->move(0, dy, 0);
	for( AUTO_VAR(it, pieces.begin()); it != pieces.end(); it++ )
	{
		StructurePiece *piece = *it;
        piece->getBoundingBox()->move(0, dy, 0);
    }
}

void StructureStart::moveInsideHeights(Level *level, Random *random, int lowestAllowed, int highestAllowed)
{
    int heightSpan = highestAllowed - lowestAllowed + 1 - boundingBox->getYSpan();
    int y0Pos = 1;

    if (heightSpan > 1)
	{
        y0Pos = lowestAllowed + random->nextInt(heightSpan);
    }
	else
	{
        y0Pos = lowestAllowed;
    }

    // move all bounding boxes
    int dy = y0Pos - boundingBox->y0;
    boundingBox->move(0, dy, 0);
	for( AUTO_VAR(it, pieces.begin()); it != pieces.end(); it++ )
	{
		StructurePiece *piece = *it;
        piece->getBoundingBox()->move(0, dy, 0);
    }
}

bool StructureStart::isValid()
{
	return true;
}