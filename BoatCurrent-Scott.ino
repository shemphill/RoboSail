/* BoatCodeStarterWind+GPS+Compass rev 7/31/2017
© 2014-2017 RoboSail
Full description under Description tab */

boolean displayValues = false;  //true calls function for values to be printed to monitor
boolean logValues = true;  //true calls function for values to be printed to log

#define USE_GPS
#include <Servo.h>
#ifdef USE_GPS
#include <Adafruit_GPS.h>
#endif
#include <SoftwareSerial.h>
#include <PreciseCalcs.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include "TimeOut.h"
#include "SDlog.h"
#include "RoboSail.h"
#include "PIDcontrol.h"
//Fill in min/max parameters for the RC Receiver and WindSensor in RoboSail.h tab

#ifdef USE_GPS
Adafruit_GPS GPS(&Serial1);
// initialize utility that will convert lat/lon to (x,y) positions in meters
PreciseCalcs calc;
#endif

const int commandStackSize = 10;
struct command {
  void (*func)(void);
  unsigned long timeout;
  bool immed;
} commandStack[commandStackSize];
struct command *commandPtr = commandStack;

// Note: the following unique characters are written to the SD log to distinguish which
// routine is making the log entry:
// @ command
// # heel correction
// $ sail to wind
// % sail to compass
// & sail square
// | sculling

void setup() {
  Serial.begin(115200);
  accel.begin();
  mag.begin();
  Serial.println("Scott's BoatCode Wind+GPS+Compass-10/28/17");  //write program name/rev here
  Log.println("Scott's BoatCode Wind+GPS+Compass-10/28/17");  //write program name/rev here
  // Set RC receiver and all Sensors on digital input pins
  declarePins();

#ifdef USE_GPS
  checkGPS();
#endif

  checkCompass();
 }

void readBoat()
{
  const int windCalibrate = 100;  // this should be positive
  readReceiver();
  readWind();
  // adjust wind
  windAngle = ((windAngle + windCalibrate + 180) % 360) - 180;
  readAccel();

#ifdef USE_GPS
  readGPS();  //puts values in "start" and "relative" variable
#endif
  
  //Read heading and tilt from the Compass
  readCompassAccel();
  
}

void writeBoat()
{
  driveSailServo(sailPosition);
  driveRudderServo(rudderPosition);
  
  static TimeOut timeOut(3000);
  if (displayValues && timeOut()) {printToMonitor();} 
  if (logValues) {printToLog();} 
}

// Everything is commented out now, so that manualDrive just passes the rudder and sail positions
// from the RC controller directly to the driver

inline void manualDrive()
{
//  static int oldRudderPosition;
//  rudderPosition = (rudderPosition + oldRudderPosition*4) / 5; // low pass filter on rudder position
//  oldRudderPosition = rudderPosition;
}

// try to obtain the sail position in "sailPosition", but let the
// sheets out if the heel angle is too much

void safeSailPosition()
{
  const bool logcorrection = true;
  const float safeHeel = 9.80665 * sin(30*DEG_TO_RAD); // 30 degrees is OK
  const float heelKp = -20.0;
  static PIDcontrol<float> pid(heelKp);
  static bool sustained;
  float heel = fabs(robosailRollAccel);
  if (!sustained) pid.reset();
  float corr = pid.output(safeHeel-heel);  // calculate sail correction
  if (corr < 0) {  // no correction needed
    if (sustained) {
      sustained = false;
      if (logcorrection) {
	Log.print("#  Heel: ");
	Log.println(heel);
	Log.println("#Ending heel correction");
      }
    }
    return;
  }
  if (!sustained) {
    sustained = true;
    if (logcorrection) Log.println("#Beginning heel correction");
  }
  if (logcorrection) {
    Log.print("#  Heel: ");
    Log.print(heel);
    Log.print("  amount of correction: ");
    Log.println(corr);
  }
  sailPosition += corr;
  constrain(sailPosition, 0, 90);
}

void sailFollowsWind()
{
  sailPosition = map(abs(windAngle), 30, 120, 0, 90);
  sailPosition = constrain(sailPosition, 0, 90);
  safeSailPosition();
}

bool scullEverywhere = false;

void scullIfIrons()
{
  if (scullEverywhere || abs(windAngle) < 30) {
    const bool logsculling = true;
    static TimeOut timeOut(200);
    static int last = -20;
    if (timeOut()) {
      last = -last;
      if (logsculling) {
	if (last < 0) Log.println("|sculling left"); else Log.println("|sculling right");
      }
    }
    rudderPosition += last;
    rudderPosition = constrain(rudderPosition, -60, 60);
  }
}

void sailToWind(int targetWindAngle)
{
  const float sailKp = 1.0;
  const bool logRudder = true;
  const bool printRudder = false;
  PIDcontrol<float> pid(sailKp);
  int error = (targetWindAngle - windAngle + 540) % 360 - 180;
  rudderPosition = pid.output(error);
  if (printRudder) {
    unsigned long int lastTime, now;
    now = millis();
    if (now - lastTime >= 1000) {
      Log.print("$targetWindAngle: ");
      Log.print(targetWindAngle);
      Log.print(", heading: ");
      Log.print(heading);
      Log.print(", error: ");
      Log.print(error);
      Log.print(", unconstrained rudderPosition: ");
      Log.println(rudderPosition);
      lastTime = now;
    }
  } 
  if (logRudder) {
    Log.print("$targetWindAngle: ");
    Log.print(targetWindAngle);
    Log.print(", windAngle: ");
    Log.print(windAngle);
    Log.print(", error: ");
    Log.print(error);
    Log.print(", unconstrained rudderPosition: ");
    Log.println(rudderPosition);
  } 
  rudderPosition = constrain(rudderPosition, -60, 60);
  scullIfIrons();
  sailFollowsWind();
}

void sailToCompass(int targetHeading)
{
  const float sailKp = 1.0;
  const bool logRudder = true;
  const bool printRudder = false;
  PIDcontrol<float> pid(sailKp);
  int error = (targetHeading - heading + 540) % 360 - 180;  // using real heading, not robosailHeading
  rudderPosition = pid.output(error);
  if (printRudder) {
    unsigned long int lastTime, now;
    now = millis();
    if (now - lastTime >= 1000) {
      Serial.print("%targetHeading: ");
      Serial.print(targetHeading);
      Serial.print(", heading: ");
      Serial.print(heading);
      Serial.print(", error: ");
      Serial.print(error);
      Serial.print(", unconstrained rudderPosition: ");
      Serial.println(rudderPosition);
      lastTime = now;
    }
  } 
  if (logRudder) {
    Log.print("%targetHeading: ");
    Log.print(targetHeading);
    Log.print(", heading: ");
    Log.print(heading);
    Log.print(", error: ");
    Log.print(error);
    Log.print(", unconstrained rudderPosition: ");
    Log.println(rudderPosition);
  } 
  rudderPosition = constrain(rudderPosition, -60, 60);
  scullIfIrons();
  sailFollowsWind();
}

void sailSquare()
{
  const bool logsquare = true;
  static bool insquare;
  static unsigned long int entryTime;
  if (!insquare) {
    insquare = true;
    entryTime = millis();
  }
  switch ((millis()-entryTime)/15000) {
  case 0:
    if (logsquare) Log.println("&Beginning turn to north");
    sailToCompass(0);
    break;
  case 1:
    if (logsquare) Log.println("&Beginning turn to west");
    sailToCompass(270);
    break;
  case 2:
    if (logsquare) Log.println("&Beginning turn to south");
    sailToCompass(180);
    break;
  case 3:
    if (logsquare) Log.println("&Beginning turn to east");
    sailToCompass(90);
    break;
  case 4:
    if (logsquare) Log.println("&Beginning turn back to north");
    sailToCompass(0);
    break;
  default:
    insquare = false;
    command = 0;
    return;
  }
}

void commandDispatch()
{
  if (manualOverride) {
    manualDrive();
    return;
  }
  switch (command) {
  case 0:
    scullIfIrons();
    sailFollowsWind();
    break;
  case 1:
    scullEverywhere = true;
    command = lastcommand;
    return;
  case 2:
    scullEverywhere = false;
    command = lastcommand;
    return;
  case 3:
    scullIfIrons();
    sailFollowsWind();
    break;
  case 4 ... 9:
    static const int windAngleTable[] = { 45, 90, 160, -160, -90, -45, };
    sailToWind(windAngleTable[command-4]);
    break;
  case 10 ... 13:
    static const int headingTable[] = { 0, 270, 180, 90, };
    sailToCompass(headingTable[command-10]);
    break;
  case 14:
    sailSquare();
    break;
  default:
    Log.print("@unknown command ");
    Log.println(command);
    command = lastcommand;
    commandDispatch();
    return;
  }
}

void loop() {
  readBoat();
  getCommand();
  commandDispatch();
  writeBoat();
} //end of loop()

