#pragma once

class MaterialColor
{
public:
    static MaterialColor **colors;
		   
    static MaterialColor *none;
    static MaterialColor *grass;
    static MaterialColor *sand;
    static MaterialColor *cloth;
    static MaterialColor *fire;
    static MaterialColor *ice;
    static MaterialColor *metal;
    static MaterialColor *plant;
    static MaterialColor *snow;
    static MaterialColor *clay;
    static MaterialColor *dirt;
    static MaterialColor *stone;
    static MaterialColor *water;
    static MaterialColor *wood;

	static void staticCtor();

public:
    eMinecraftColour col;
    int id;

private:
	MaterialColor(int id, eMinecraftColour col);
};
