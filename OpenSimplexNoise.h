#ifndef _NOISE_H_
#define _NOISE_H_

class OpenSimplexNoise
{
private:
  short perm[256];
  short permGradIndex3D[256];

  double extrapolate(int xsb, int ysb, double dx, double dy);
  double extrapolate(int xsb, int ysb, int zsb, double dx, double dy, double dz);
  double extrapolate(int xsb, int ysb, int zsb, int wsb, double dx, double dy, double dz, double dw);
  int fastFloor(double x);

public:
  OpenSimplexNoise();
  OpenSimplexNoise(short p[256]);
  OpenSimplexNoise(long seed);

  double eval(double x, double y);
  double eval(double x, double y, double z);
  double eval(double x, double y, double z, double w);

};

#endif