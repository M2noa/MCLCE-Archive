#pragma once
#include "CompoundTag.h"
#include "Tag.h"
#include "CompoundTag.h"

class InputStream;

class NbtIo
{
public:
    static CompoundTag *readCompressed(InputStream *in);
    static void writeCompressed(CompoundTag *tag, OutputStream *out);
    static CompoundTag *decompress(byteArray buffer);
    static byteArray compress(CompoundTag *tag);
    static CompoundTag *read(DataInput *dis);
    static void write(CompoundTag *tag, DataOutput *dos);
};
