#pragma once
class BufferedImage;

class HttpTextureProcessor
{
public:
	virtual BufferedImage *process(BufferedImage *read) = 0;
};