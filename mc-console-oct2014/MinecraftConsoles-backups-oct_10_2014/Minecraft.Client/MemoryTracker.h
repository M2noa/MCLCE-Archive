#pragma once
#include "MemoryTracker.h"
class ByteBuffer;
class IntBuffer;
class FloatBuffer;
using namespace std;

/** Original comment
 * This class is used so we can release all memory (allocated on the graphics card on shutdown)
 */
// 4J - all member functions in here were synchronized
class MemoryTracker
{
private:
	static unordered_map<int,int> GL_LIST_IDS;
    static vector<int> TEXTURE_IDS;

public:
	static int genLists(int count);
    static int genTextures();
	static void releaseLists(int id);
	static void releaseTextures();
    static void release();
	// 4J - note - have removed buffer types from here that we aren't using
    static ByteBuffer *createByteBuffer(int size);
    static IntBuffer *createIntBuffer(int size);
    static FloatBuffer *createFloatBuffer(int size);
};
