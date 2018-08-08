#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include "metersperdegree.h"

//#define DEBUG
#define abs(x) ((x) < 0 ? -(x) : (x))

char *prog;

void usage(void)
{
  fprintf(stderr, "usage: %s [logfile]\n", prog);
  exit(1);
}

const int XSIZE = 1920;
const int YSIZE = 1087;

const int table[] = {
  1, 2, 5, 10, 20, 30,						// seconds
  60, 120, 300, 600, 1200, 1800,				// minutes
  3600, 7200, 18000, 36000, 72000, 108000, 216000, 324000,	// degrees
};
const int NTABLE = sizeof(table)/sizeof(int);

int main(int argc, char *argv[])
{
  FILE *f = stdin;
  char buf[BUFSIZ];
  char *p;
  int maxlat, maxlong, minlat, minlong;
  int currentlat, currentlong;
  double middlelat, middlelong;
  double MPDlat, MPDlong;
  double maplat, maplong;
  double mapalt;
  int latchar, longchar;
  int i;
  prog = argv[0];
  if (argc > 2) usage();
  if (argc == 2) f = fopen(argv[1], "r");
  if (!f) usage();
  minlat = minlong = INT_MAX;
  maxlat = maxlong = INT_MIN;
  while (fgets(buf, BUFSIZ, f)) {
    p = strstr(buf, "currentpos = ");
    if (!p) continue;
    sscanf(p+13, "%d,%d", &currentlong, &currentlat);
    if (currentlat > maxlat) maxlat = currentlat;
    if (currentlong > maxlong) maxlong = currentlong;
    if (currentlat < minlat) minlat = currentlat;
    if (currentlong < minlong) minlong = currentlong;
  }
  if (maxlat < minlat || maxlong < minlong) {
    fprintf(stderr, "no fixes found\n");
    exit(1);
  }
  middlelat = 0.5e-7*(maxlat+minlat);
  middlelong = 0.5e-7*(maxlong+minlong);
  metersperdegree(middlelat, &MPDlat, &MPDlong);
  for (i = 0; i < NTABLE-1; i++) {
    double xsize, ysize;
    double xsizedeg, ysizedeg;
    double safelatmax, safelatmin, safelongmax, safelongmin;
    maplat = floor(middlelat*3600.0/table[i]+0.5) * table[i]/3600.0;
    maplong = floor(middlelong*3600.0/table[i]+0.5) * table[i]/3600.0;
    mapalt = 100*table[i];
    ysize = mapalt*2/3.0;
    xsize = ysize*XSIZE/(double)YSIZE;
    xsizedeg = xsize / MPDlong;
    ysizedeg = ysize / MPDlat;
    safelatmax = maplat + 0.5*0.5625*ysizedeg;
    safelatmin = maplat - 0.5*0.5625*ysizedeg;
    safelongmax = maplong + 0.5*0.5625*xsizedeg;
    safelongmin = maplong - 0.5*0.5625*xsizedeg;
#ifdef DEBUG
    double maplatmax, maplatmin, maplongmax, maplongmin;
    maplatmax = maplat + 0.5*ysizedeg;
    maplatmin = maplat - 0.5*ysizedeg;
    maplongmax = maplong + 0.5*xsizedeg;
    maplongmin = maplong - 0.5*xsizedeg;
    fprintf(stderr,"------------------------------------------------------------------------\n");
    fprintf(stderr, "i = %d\n", i);
    fprintf(stderr, "round to the nearest %d minutes\n", table[i]);
    fprintf(stderr, "original coordinates = (%.17g,%.17g)\n", middlelong, middlelat);
    fprintf(stderr, "rounded coordinates = (%.17g,%.17g)\n", maplong, maplat);
    fprintf(stderr, "eye height = %d meters\n", mapalt);
    fprintf(stderr, "map size in meters = %.17g x %.17g\n", xsize, ysize);
    fprintf(stderr, "map size in degrees = %.17g x %.17g\n", xsize/MPDlong, ysize/MPDlat);
    fprintf(stderr, "map latitude range = %.17g to %.17g\n", maplatmin, maplatmax);
    fprintf(stderr, "map longitude range = %.17g to %.17g\n", maplongmin, maplongmax);
    fprintf(stderr, "safe latitude range = %.17g to %.17g\n", safelatmin, safelatmax);
    fprintf(stderr, "safe longitude range = %.17g to %.17g\n", safelongmin, safelongmax);
    fprintf(stderr, "course latitude range = %.17g to %.17g\n", 1e-7*minlat, 1e-7*maxlat);
    fprintf(stderr, "course longitude range = %.17g to %.17g\n", 1e-7*minlong, 1e-7*maxlong);
    fprintf(stderr, "course latitude max as a fraction of safe = %.17g\n",
      (1e-7*maxlat-maplat)/(0.5*0.5625*ysizedeg));
    fprintf(stderr, "course latitude min as a fraction of safe = %.17g\n",
      (maplat-1e-7*minlat)/(0.5*0.5625*ysizedeg));
    fprintf(stderr, "course longitude max as a fraction of safe = %.17g\n",
      (1e-7*maxlong-maplong)/(0.5*0.5625*xsizedeg));
    fprintf(stderr, "course longitude min as a fraction of safe = %.17g\n",
      (maplong-1e-7*minlong)/(0.5*0.5625*xsizedeg));
#endif
    if (1e-7*maxlat > safelatmax) continue;
    if (1e-7*minlat < safelatmin) continue;
    if (1e-7*maxlong > safelongmax) continue;
    if (1e-7*minlong < safelongmin) continue;
    break;
  }
  printf("suggested map location: ");
  currentlat = floor(maplat*3600.0+0.5);
  currentlong = floor(maplong*3600.0+0.5);
  latchar = currentlat < 0 ? 'S' : 'N';
  longchar = currentlong < 0 ? 'W' : 'E';
  currentlat = abs(currentlat);
  currentlong = abs(currentlong);
  printf("%d.%02d%02d%c", currentlat/3600, (currentlat/60)%60, currentlat%60, latchar);
  printf("%d.%02d%02d%c", currentlong/3600, (currentlong/60)%60, currentlong%60, longchar);
  printf("%g\n", mapalt);
  return 0;
}
