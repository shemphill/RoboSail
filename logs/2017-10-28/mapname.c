#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "metersperdegree.h"

#define XSIZE (1920-204)
#define YSIZE (1106-44)

double interp(double t, double t1, double u1, double t2, double u2)
{
  return (u2*(t-t1)+u1*(t2-t)) / (t2-t1);
}

int main(void)
{
  char buf[BUFSIZ];
  fprintf(stderr, "enter two positions in (y,x) pixel coordinates:\n");
  int x1,y1,x2,y2;
  do {
    fprintf(stderr, "position 1: ");
    fgets(buf, BUFSIZ, stdin);
  } while (sscanf(buf, "%d %d", &y1, &x1) != 2);
  do {
    fprintf(stderr, "position 2: ");
    fgets(buf, BUFSIZ, stdin);
  } while (sscanf(buf, "%d %d", &y2, &x2) != 2);
  fprintf(stderr, "enter those positions in latitude and longitude:\n");
  double lat1,long1,lat2,long2;
  do {
    fprintf(stderr, "position 1: ");
    fgets(buf, BUFSIZ, stdin);
  } while (sscanf(buf, "%lf %lf", &lat1, &long1) != 2);
  do {
    fprintf(stderr, "position 2: ");
    fgets(buf, BUFSIZ, stdin);
  } while (sscanf(buf, "%lf %lf", &lat2, &long2) != 2);
  printf("cbi");
  double middleLat;
  double MPDlat, MPDlong;
  double metersperpixel;
  middleLat = interp(YSIZE/2.0-0.5, y1, lat1, y2, lat2);
  metersperdegree(middleLat, &MPDlat, &MPDlong);
  metersperpixel = MPDlat * (lat2 - lat1) / (y2 - y1);
  printf("%.17gN", fabs(interp(YSIZE/2.0-0.5, y1, lat1, y2, lat2)));
  printf("%.17gW", fabs(interp(XSIZE/2.0-0.5, x1, long1, x2, long2)));
  printf("%.17g\n", fabs(metersperpixel));
  return 0;
}
