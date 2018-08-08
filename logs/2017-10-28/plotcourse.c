#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include "metersperdegree.h"

#define XADJUST -100
#define YADJUST -200
#define XARROW 0
#define YARROW -72

#define max(x,y) ((x) > (y) ? (x) : (y))
#define min(x,y) ((x) < (y) ? (x) : (y))
#define sqr(x) ((x)*(x))

#define DEG_TO_RAD 0.017453292519943295769
#define RAD_TO_DEG 57.295779513082320877
#define fname "cbi42.359179544397954N71.073616244212971W0.067133585232477339.ppm"

double centerLat = 42.359179544397954;
double centerLong = -71.073616244212971;
double metersperpixel = 0.067133585232477339;

char *prog;

void usage(void)
{
  fprintf(stderr, "usage: %s logfile\n", prog);
  exit(1);
}

char *header = "%!PS\n"
"<</Orientation 1>> setpagedevice\n"
"/in { 72 mul } bind def\n"
"4.25 in 5.5 in translate\n"
"90 rotate\n"
"/ltblue { .33 .57 .75 } def\n"
"/ltgreen { .63 .79 .52 } def\n"
"/green { 0 1 0 } def\n"
"/ltyellow { 1 .97 .61 } def\n"
"/yellow { 1 1 0 } def\n"
"/redpurple { .53 .14 .30 } def\n"
"/red { 1 0 0 } def\n"
"/orange { 1 0.5 0 } def\n"
"/gray { 0.3 0.3 0.3 } def\n"
"green setrgbcolor\n"
"/f currentfile /ASCIIHexDecode filter def\n"
"/arrowlen 8 def\n"
"/arrowwidth 4 def\n"
"/arrowto {\n"
"  /y2 exch def /x2 exch def\n"
"  currentpoint /y1 exch def /x1 exch def\n"
"  /angle1 y2 y1 sub x2 x1 sub atan def\n"
"  /len x1 x2 sub dup mul y1 y2 sub dup mul add sqrt arrowlen sub def\n"
"  len angle1 cos mul len angle1 sin mul rlineto currentpoint stroke moveto\n"
"  arrowwidth 0.5 mul dup angle1 sin mul exch neg angle1 cos mul 2 copy rmoveto\n"
"  -2 mul exch -2 mul exch rlineto\n"
"  x2 y2 lineto\n"
"  closepath fill\n"
"} bind def\n"

;

char *trailer = ""
"showpage\n"
;

void maperror(char *s)
{
  fprintf(stderr, "%s\n", s);
  exit(1);
}

char *chartohex(int ch)
{
  static char buf[3];
  int x = (ch>>4) & 0xf;
  if (x < 10) buf[0] = x + '0';
  else buf[0] = x - 10 + 'a';
  x = ch & 0xf;
  if (x < 10) buf[1] = x + '0';
  else buf[1] = x - 10 + 'a';
  return buf;
}

double MPDlat, MPDlong;
double pointspermeter;
double midx;
double midy;

void printfile(FILE *f)
{
  int xsize, ysize;
  char buf[BUFSIZ];
  fgets(buf, BUFSIZ, f);
  if (strcmp(buf, "P6\n")) maperror("map image file is not a .ppm file");
  fgets(buf, BUFSIZ, f);
  if (sscanf(buf, "%d %d", &xsize, &ysize) != 2)
    maperror("couldn't parse size of map image file");
  fgets(buf, BUFSIZ, f);
  if (strcmp(buf, "255\n")) maperror("invalid map image color depth");
  printf("gsave\n");
  printf("%f %f translate\n",
    (centerLong-midx)*MPDlong*pointspermeter,
    (centerLat-midy)*MPDlat*pointspermeter);
  printf("%f %f scale\n",
    xsize*metersperpixel*pointspermeter, ysize*metersperpixel*pointspermeter);

  printf("-0.5 -0.5 translate\n");
  printf("{ %d %d 8 [%d 0 0 -%d 0 %d] f false 3 colorimage } exec\n",
    xsize, ysize, xsize, ysize, ysize);

  int pos = 0;
  int i, j;
  for (i = 0; i < ysize; i++) {
    for (j = 0; j < xsize; j++) {
      int ch = fgetc(f);
      if (ch == EOF) maperror("premature end of map image file");
      fputs(chartohex(ch), stdout);
      ch = fgetc(f);
      if (ch == EOF) maperror("premature end of map image file");
      fputs(chartohex(ch), stdout);
      ch = fgetc(f);
      if (ch == EOF) maperror("premature end of map image file");
      fputs(chartohex(ch), stdout);
      pos += 6;
      if (pos >= 78) {
	pos = 0;
	fputc('\n', stdout);
      }
    }
  }
  if (pos >= 78) {
    pos = 0;
    fputc('\n', stdout);
  }
  printf("grestore\n");
}

int main(int argc, char *argv[])
{
  FILE *f = stdin;
  char buf[BUFSIZ];
  prog = argv[0];
  int now, startx, starty, currentx, currenty;
  double minx = INT_MAX, maxx = INT_MIN, miny = INT_MAX, maxy = INT_MIN;
  if (argc != 2) usage();
  f = fopen(argv[1], "r");
  if (!f) usage();
  while (fgets(buf, BUFSIZ, f)) {
    if (sscanf(buf, "%d, Fix: startpos = %d,%d currentpos = %d,%d",
	       &now, &startx, &starty, &currentx, &currenty) != 5) continue;
    currentx += XADJUST;
    currenty += YADJUST;
    if (currentx < minx) minx = currentx;
    if (currentx > maxx) maxx = currentx;
    if (currenty < miny) miny = currenty;
    if (currenty > maxy) maxy = currenty;
  }
  minx *= 1e-7;
  maxx *= 1e-7;
  miny *= 1e-7;
  maxy *= 1e-7;
  double xrange = maxx - minx;
  double yrange = maxy - miny;
  midx = 0.5*(minx+maxx);
  midy = 0.5*(miny+maxy);
  metersperdegree(midy, &MPDlat, &MPDlong);
  double xpointspermeter = 11*72*0.75/xrange / MPDlong;
  double ypointspermeter = 8.5*72*0.75/yrange / MPDlat;
  pointspermeter = min(xpointspermeter, ypointspermeter);

  FILE *g = fopen(fname, "r");
  if (!g) maperror("couldn't open map image file");

  fputs(header, stdout);
  printfile(g);  // display background map image

  rewind(f);
  printf("25 setlinewidth gray setrgbcolor\n");
  printf("/arrowlen 100 def\n");
  printf("/arrowwidth 50 def\n");
  double sumx = 0, sumy = 0;
  double temp;
  while (fgets(buf, BUFSIZ, f)) {
    double heading;
    double windangle;
    char *p;
    if ((p = strstr(buf, "Wind Angle:"))) {
      sscanf(p+11, "%lf", &windangle);
      continue;
    }
    if ((p = strstr(buf, "Actual heading:"))) {
      sscanf(p+15, "%lf", &heading);
      sumx -= sin((heading-windangle)*DEG_TO_RAD);
      sumy -= cos((heading-windangle)*DEG_TO_RAD);
    }
  }
  temp = sqrt(sqr(sumx)+sqr(sumy));
  sumx /= temp;
  sumy /= temp;
  printf("%f %f moveto %f %f arrowto\n", XARROW-100*sumx, YARROW-100*sumy, XARROW+100*sumx, YARROW+100*sumy);

  rewind(f);
  printf("3 setlinewidth red setrgbcolor\n");
  while (fgets(buf, BUFSIZ, f)) {
    double xpos, ypos;
    int heeling;
    static int moved = 0;
    if (strstr(buf, "Beginning heel correction")) heeling = 1;
    else if (strstr(buf, "Ending heel correction")) heeling = 0;
    if (sscanf(buf, "%d, Fix: startpos = %d,%d currentpos = %d,%d",
	       &now, &startx, &starty, &currentx, &currenty) != 5) continue;
    if (!heeling) { moved = 0; continue; }
    currentx += XADJUST;
    currenty += YADJUST;
    xpos = (1e-7*currentx - midx) * MPDlong * pointspermeter;
    ypos = (1e-7*currenty - midy) * MPDlat * pointspermeter;
    if (moved++) printf("%f %f lineto stroke\n", xpos, ypos);
    printf("%f %f moveto\n", xpos, ypos);
  }

  rewind(f);
  printf("2 setlinewidth orange setrgbcolor\n");
  printf("/arrowlen 8 def\n");
  printf("/arrowwidth 4 def\n");
  while (fgets(buf, BUFSIZ, f)) {
    double xpos, ypos;
    double heading, dx, dy;
    int arrownow;
    static int arrownext = 30000;
    char *p;
    if ((p = strstr(buf, "Actual heading:"))) {
      sscanf(buf, "%d", &arrownow);
      sscanf(p+15, "%lf", &heading);
    }
    if (sscanf(buf, "%d, Fix: startpos = %d,%d currentpos = %d,%d",
	       &now, &startx, &starty, &currentx, &currenty) != 5) continue;
    if (arrownow < arrownext) continue;
    arrownext += 30000;
    currentx += XADJUST;
    currenty += YADJUST;
    xpos = (1e-7*currentx - midx) * MPDlong * pointspermeter;
    ypos = (1e-7*currenty - midy) * MPDlat * pointspermeter;
    dx = sin(heading*DEG_TO_RAD);
    dy = cos(heading*DEG_TO_RAD);
    printf("%f %f moveto %f %f arrowto\n", xpos-3*dx, ypos-3*dy, xpos+11*dx, ypos+11*dy);
  }

  rewind(f);
  printf("1 setlinewidth green setrgbcolor\n");
  while (fgets(buf, BUFSIZ, f)) {
    double xpos, ypos;
    static int moved = 0;
    static int commandnum = -2;
    char *p;
    if ((p = strstr(buf, "command"))) {
      int temp;
      if (sscanf(p+7, "%d", &temp) != 1) temp = -1;
      if (temp != commandnum) {
	switch ((commandnum = temp)) {
	case -1:
	  printf("green setrgbcolor\n");
	  break;
	case 0:
	  printf("ltblue setrgbcolor\n");
	  break;
	default:
	  printf("yellow setrgbcolor\n");
	  break;
	}
      }
      continue;
    }
    if (sscanf(buf, "%d, Fix: startpos = %d,%d currentpos = %d,%d",
	       &now, &startx, &starty, &currentx, &currenty) != 5) continue;
    currentx += XADJUST;
    currenty += YADJUST;
    xpos = (1e-7*currentx - midx) * MPDlong * pointspermeter;
    ypos = (1e-7*currenty - midy) * MPDlat * pointspermeter;
    if (moved++) printf("%f %f lineto stroke\n", xpos, ypos);
    printf("%f %f moveto\n", xpos, ypos);
  }
  fputs(trailer, stdout);
  return 0;
}
