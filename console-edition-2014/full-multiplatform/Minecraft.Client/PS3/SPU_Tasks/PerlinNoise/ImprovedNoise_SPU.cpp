#include "stdafx.h"
#include "ImprovedNoise_SPU.h"




double ImprovedNoise_SPU::lerp(double t, double a, double b)
{
   return a + t * (b - a);
}

double ImprovedNoise_SPU::grad2(int hash, double x, double z)
{
    int h = hash & 15; // CONVERT LO 4 BITS OF HASH CODE
        
    double u = (1-((h&8)>>3))*x, // INTO 12 GRADIENT DIRECTIONS.
    v = h < 4 ? 0 : h == 12 || h == 14 ? x : z;
                
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

double ImprovedNoise_SPU::grad(int hash, double x, double y, double z)
{
    int h = hash & 15; // CONVERT LO 4 BITS OF HASH CODE
        
    double u = h < 8 ? x : y, // INTO 12 GRADIENT DIRECTIONS.
    v = h < 4 ? y : h == 12 || h == 14 ? x : z;
                
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}


void ImprovedNoise_SPU::add(double* buffer, double _x, double _y, double _z, int xSize, int ySize, int zSize, double xs, double ys, double zs, double pow)
{
    if (ySize==1)
	{
        int A = 0, AA = 0, B = 0, BA = 0;
        double vv0 = 0, vv2 = 0;
        int pp = 0;
        double scale = 1.0 / pow;
        for (int xx = 0; xx < xSize; xx++)
		{
            double x = _x + (xx) * xs + xo;
            int xf = (int) x;
            if (x < xf) xf--;
            int X = xf & 255;
            x -= xf;
            double u = x * x * x * (x * (x * 6 - 15) + 10);

            for (int zz = 0; zz < zSize; zz++)
			{
                double z = _z + (zz) * zs + zo;
                int zf = (int) z;
                if (z < zf) zf--;
                int Z = zf & 255;
                z -= zf;
                double w = z * z * z * (z * (z * 6 - 15) + 10);
                    
                A = p[X] + 0;
                AA = p[A] + Z;
                B = p[X + 1] + 0;
                BA = p[B] + Z;
                vv0 = lerp(u, grad2(p[AA], x, z), grad(p[BA], x - 1, 0, z));
                vv2 = lerp(u, grad(p[AA + 1], x, 0, z - 1), grad(p[BA + 1], x - 1, 0, z - 1));

                double val = lerp(w, vv0, vv2);

                buffer[pp++] += val * scale;
            }
        }
        return;
    }
    int pp = 0;
    double scale = 1 / pow;
    int yOld = -1;
    int A = 0, AA = 0, AB = 0, B = 0, BA = 0, BB = 0;
    double vv0 = 0, vv1 = 0, vv2 = 0, vv3 = 0;

    for (int xx = 0; xx < xSize; xx++)
	{
        double x = _x + (xx) * xs + xo;
        int xf = (int) x;
        if (x < xf) xf--;
        int X = xf & 255;
        x -= xf;
        double u = x * x * x * (x * (x * 6 - 15) + 10);


        for (int zz = 0; zz < zSize; zz++)
		{
            double z = _z + (zz) * zs + zo;
            int zf = (int) z;
            if (z < zf) zf--;
            int Z = zf & 255;
            z -= zf;
            double w = z * z * z * (z * (z * 6 - 15) + 10);


            for (int yy = 0; yy < ySize; yy++)
			{
                double y = _y + (yy) * ys + yo;
                int yf = (int) y;
                if (y < yf) yf--;
                int Y = yf & 255;
                y -= yf;
                double v = y * y * y * (y * (y * 6 - 15) + 10);

                if (yy == 0 || Y != yOld)
				{
                    yOld = Y;
                    A = p[X] + Y;
                    AA = p[A] + Z;
                    AB = p[A + 1] + Z;
                    B = p[X + 1] + Y;
                    BA = p[B] + Z;
                    BB = p[B + 1] + Z;
                    vv0 = lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z));
                    vv1 = lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z));
                    vv2 = lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1));
                    vv3 = lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1));
                }


                double v0 = lerp(v, vv0, vv1);
                double v1 = lerp(v, vv2, vv3);
                double val = lerp(w, v0, v1);

                buffer[pp++] += val * scale;
            }
        }
	}
}
