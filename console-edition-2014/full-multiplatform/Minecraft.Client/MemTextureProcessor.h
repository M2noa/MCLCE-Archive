#pragma once
class BufferedImage;

class MemTextureProcessor
{
public:
	virtual BufferedImage *process(BufferedImage *read) = 0;
};