#pragma once

class SimplexNoise
{
private:
    static int grad3[12][3];

    int *p;

    static double F2;
    static double G2;
	static double F3;
    static double G3;

public:
    double scale;
    double xo, yo, zo;

    SimplexNoise();
    SimplexNoise(Random *random);
	void init(Random *random);
	~SimplexNoise();

    // This method is a *lot* faster than using (int)Math.floor(x)
private:
    static int fastfloor(double x);
    static double dot(int *g, double x, double y);
    static double dot(int *g, double x, double y, double z);

    // 2D simplex noise
public:
    double getValue(double xin, double yin);

    // 3D simplex noise
    double getValue(double xin, double yin, double zin);

    void add(doubleArray buffer, double _x, double _y, int xSize, int ySize, double xs, double ys, double pow);
    void add(doubleArray buffer, double _x, double _y, double _z, int xSize, int ySize, int zSize, double xs, double ys, double zs, double pow);
    
};
