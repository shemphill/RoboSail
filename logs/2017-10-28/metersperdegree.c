#include <math.h>
#include "metersperdegree.h"

void metersperdegree(double degLat, double *MPDlat, double *MPDlong)
{
  const double DEG_TO_RAD = 0.017453292519943295769;
  const double c1 = 0.0067394967565868823004;
  const double c2 = 111693.97955992134774;
  double cosine = cos(degLat * DEG_TO_RAD);
  double denom = 1 + c1*cosine*cosine;
  double sqrtDenom = sqrt(denom);
  *MPDlat = c2 / (denom*sqrtDenom);
  *MPDlong = c2 * cosine / sqrtDenom;
}

#ifdef MAIN

#include <stdio.h>
#include <stdlib.h>

char *prog;

void usage(void)
{
  fprintf(stderr, "usage: %s latitude\n", prog);
  exit(1);
}

int main(int argc, char *argv[])
{
  prog = argv[0];
  if (argc != 2) usage();
  double degLat;
  char *p;
  degLat = strtod(argv[1], &p);
  if (p == argv[1] && *p) usage();
  double MPDlat, MPDlong;
  metersperdegree(degLat, &MPDlat, &MPDlong);
  printf("meters per degree of latitude = %.17g\n", MPDlat);
  printf("meters per degree of longitude = %.17g\n", MPDlong);
  return 0;
}

#endif
