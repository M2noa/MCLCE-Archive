#pragma once
class StructurePiece;
class BoundingBox;

class StructureStart
{

protected:
	list<StructurePiece *> pieces;
    BoundingBox *boundingBox;

    StructureStart();
public:
	~StructureStart();
	BoundingBox *getBoundingBox();
    list<StructurePiece *> *getPieces();
    void postProcess(Level *level, Random *random, BoundingBox *chunkBB);
protected:
	void calculateBoundingBox();
    void moveBelowSeaLevel(Level *level, Random *random, int offset);
	void moveInsideHeights(Level *level, Random *random, int lowestAllowed, int highestAllowed);
public:
	bool isValid();

};
