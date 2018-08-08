#ifndef _PRECISECALCS_H_
#define _PRECISECALCS_H_

#include "Adafruit_GPS.h"

class PreciseCalcs
{
  public:
    void GPStoXY(Adafruit_GPS &GPS, float &relX, float &relY);
    void reset() { started = false; }
    long startpos[2];
    long currentpos[2];
  private:
    bool started = false;
    float lastpos[2];
    float deg_lat_to_meters;
    float deg_long_to_meters;
};

void PreciseCalcs::GPStoXY(Adafruit_GPS &GPS, float &relX, float &relY)
{
  if (!GPS.fix) {
    relX = lastpos[0];
    relY = lastpos[1];
    return;
  }
  if (GPS.longitudeDegrees < 0 && GPS.longitude_fixed > 0)
    currentpos[0] = -GPS.longitude_fixed;
  else
    currentpos[0] = GPS.longitude_fixed;
  if (GPS.latitudeDegrees < 0 && GPS.latitude_fixed > 0)
    currentpos[1] = -GPS.latitude_fixed;
  else
    currentpos[1] = GPS.latitude_fixed;
  if (!started) {
    started = true;
    startpos[0] = currentpos[0];
    startpos[1] = currentpos[1];
    const float c1 = 0.0067394967565868823004;
    const float c2 = 111693.97955992134774e-7;
    float cosine = cos(GPS.latitudeDegrees * DEG_TO_RAD);
    float denom = 1 + c1*cosine*cosine;
    float sqrtDenom = sqrt(denom);
    deg_lat_to_meters = c2 / (denom*sqrtDenom);
    deg_long_to_meters = c2 * cosine / sqrtDenom;
  }
  relX = lastpos[0] = (currentpos[0]-startpos[0]) * deg_long_to_meters;
  relY = lastpos[1] = (currentpos[1]-startpos[1]) * deg_lat_to_meters;
}

#endif
