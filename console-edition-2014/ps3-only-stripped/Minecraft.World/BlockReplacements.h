#pragma once

class BlockReplacements
{
public:
	static void staticCtor();

private:
    static byteArray replacements;

public:
    static void replace(byteArray blocks);
};