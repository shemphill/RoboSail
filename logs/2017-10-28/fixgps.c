#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "metersperdegree.h"

#define RAD_TO_DEG 57.295779513082320877

char *prog;

void usage(void)
{
  fprintf(stderr, "usage: %s [logfile]\n", prog);
  exit(1);
}

int main(int argc, char *argv[])
{
  FILE *f = stdin;
  char buf[BUFSIZ];
  prog = argv[0];
  int now, startx, starty, currentx, currenty;
  if (argc > 2) usage();
  if (argc == 2) f = fopen(argv[1], "r");
  if (!f) usage();
  while (fgets(buf, BUFSIZ, f)) {
    if (sscanf(buf, "%d, Fix: startpos = %d,%d currentpos = %d,%d",
	       &now, &startx, &starty, &currentx, &currenty) != 5) {
      fputs(buf, stdout);
      continue;
    }
    double deg_lat_to_meters;
    double deg_long_to_meters;
    metersperdegree(1e-7*starty, &deg_lat_to_meters, &deg_long_to_meters);
    double relX = (currentx-startx) * 1e-7 * deg_long_to_meters;
    double relY = (currenty-starty) * 1e-7 * deg_lat_to_meters;
    int headingFromStart = atan2(relX, relY) * RAD_TO_DEG + 360.5;
    headingFromStart %= 360;
    printf("%d, Fix: startpos = %d,%d currentpos = %d,%d   x = %.2f   y = %.2f  heading from start = %d\n",
      now, startx, starty, currentx, currenty, relX, relY, headingFromStart);
  }
  return 0;
}
