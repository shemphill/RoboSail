/*********Functions for Setup ************/
void declarePins()
{
  pinMode(RUDDER_RC_PIN, INPUT);
  pinMode(SAIL_RC_PIN, INPUT);
  pinMode(WIND_PIN, INPUT);

  // attach the servos to the proper pins
  rudderServo.attach(RUDDER_SERVO_PIN);
  sailServo.attach(SAIL_SERVO_PIN);
}

#ifdef USE_GPS
void checkGPS()
{  //Set Up GPS and wait for fix on position
  GPS.begin(9600);  //default baud rate for Adafruit MTK GPS's
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);  //setting for minimum recommended data
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);  //update rate is 1 Hz
  enableInterrupt(); // activate TIMER0 interrupt, goes off every 1 msec
  while (!GPS.fix)  //loop code will not start until GPS is ready
  { readGPS();}
}
#endif

void checkCompass()
{// Set up Compass and check that it is connected
  mag.enableAutoRange(true);
    if(!mag.begin() || !accel.begin()) //Initialize the sensor
    {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("No LSM303 Compass detected ... Check your wiring!");
    while(1); }
}

long cmap(long x, long in_min, long in_max, long out_min, long out_max)
{
  long temp = (out_max*(x-in_min) + out_min*(in_max-x)) / (in_max-in_min);
  return constrain(temp, out_min, out_max);
}

/*********Functions to read RC Transmitter/Receiver and Sensors *****/
 // Takes in the PWM signals from the RC Receiver and translate
 // them to the servo positions in degrees.
 // Takes in the PWM signals from the WindSensor and translate 
 // it to the windvane position in degrees.
 
 void readReceiver()
 {
  // Read the command pulse from the RC receiver
  rudderPulseWidth = pulseIn(RUDDER_RC_PIN, HIGH);
  sailPulseWidth = pulseIn(SAIL_RC_PIN, HIGH);
  // Calculate the servo position in degrees.
  rudderPosition = cmap(rudderPulseWidth, RUDDER_LOW, RUDDER_HIGH, -60, 60);
  sailPosition = cmap(sailPulseWidth, SAIL_LOW, SAIL_HIGH, 0, 90);
 }
 
 void readWind()
 {
  // Read values from the WindSensor
  windPulseWidth = pulseIn(WIND_PIN, HIGH);
  // Convert the wind angle to degrees from PWM.  Range -180 to +180
  windAngle = map(windPulseWidth, 0, WIND_HIGH, 180, -180);
  windAngle = constrain(windAngle, -180, 180);
 }

void readAccel()   /* Read the Accelerometer event and put data in variables */ 
{
  accel.getEvent(&event); 
  pitchAccel = event.acceleration.x;
  rollAccel = event.acceleration.y;
  yawAccel = event.acceleration.z;
  //define roll for RoboSail as rolling to Port side is positive, rolling to Starboard is negative
  robosailRollAccel  = -1 * rollAccel; 
}
/************Functions to drive Sail and Rudder servos ****************/
 // This code takes in the desired postions for the servos in degrees (as 
 // defined in RoboSail) then calculates appropriate values for the servo commands, 
 // making sure not to send the servos to impossible positions, which could 
 // damage the servo motors.
 // The Rudder servo motor ranges from 0 to 180 with 90 deg in the center
 // The Sailwinch servo is at ~55 deg when full-in, which we think of as 0 deg,
 // and ~125 deg when full out, which we think of as 90 deg

void driveSailServo(int sailPos)
{
  if ((sailPos >= 0) && (sailPos <= 90))  // the command in degrees is valid
  {
    sailServoOut = map(sailPos, 0, 90, 55, 125);
    sailServo.write(sailServoOut);
  }
  else 
  {
    Serial.print("ERROR - sail position out of range: ");
    Serial.println(sailPos);
    }
}

void driveRudderServo(int rudderPos)
{
  if ((rudderPos >= -60) && (rudderPos <= 60))
  {
    rudderServoOut = map(rudderPos, -90, 90, 0, 180);
    rudderServo.write(rudderServoOut);
  }
  else 
    {
    Serial.print("ERROR - rudder position out of range: ");
    Serial.println(rudderPos);
    }
}
/****************************************************/

// Function to Print out all values for debug.
void printToMonitor()
{
  Serial.print("Wind Angle: ");
  Serial.print(windAngle);
 
  Serial.print("\t Sail, from RC: ");
  Serial.print(sailPulseWidth);
  Serial.print("  angle out: ");
  Serial.print(sailServoOut);

  Serial.print("  Rudder, from RC: ");
  Serial.print(rudderPulseWidth);
  Serial.print("  angle out: ");
  Serial.print(rudderServoOut);
  Serial.println(); // Print a new line
 
#ifdef USE_GPS
  if (GPS.fix) Serial.print("Fix: ");
  else Serial.print("No fix: ");
  Serial.print("startpos = ");
  Serial.print(calc.startpos[0]);
  Serial.print(',');
  Serial.print(calc.startpos[1]);
  Serial.print(" currentpos = ");
  Serial.print(calc.currentpos[0]);
  Serial.print(',');
  Serial.print(calc.currentpos[1]);
  Serial.print("   x = "); Serial.print(relPositionX);
  Serial.print("   y = "); Serial.print(relPositionY);
  Serial.print("  heading from start = "); Serial.println(headingFromStart);
#endif
      
  Serial.print("Roll raw value: "); Serial.print(robosailRollAccel);
  Serial.print(", Roll in deg: "); Serial.print(robosailRoll);
  Serial.print(", Actual heading: "); Serial.println(heading);
  Serial.println();
}

// Function to Log all values for debug.
void printToLog()
{
  Log.print("Wind Angle: ");
  Log.print(windAngle);
 
  Log.print("\t Sail, from RC: ");
  Log.print(sailPulseWidth);
  Log.print("  angle out: ");
  Log.print(sailServoOut);

  Log.print("  Rudder, from RC: ");
  Log.print(rudderPulseWidth);
  Log.print("  angle out: ");
  Log.print(rudderServoOut);
  Log.println(); // Print a new line
 
#ifdef USE_GPS
  if (GPS.fix) Log.print("Fix: ");
  else Log.print("No fix: ");
  Log.print("startpos = ");
  Log.print(calc.startpos[0]);
  Log.print(',');
  Log.print(calc.startpos[1]);
  Log.print(" currentpos = ");
  Log.print(calc.currentpos[0]);
  Log.print(',');
  Log.print(calc.currentpos[1]);
  Log.print("   x = "); Log.print(relPositionX);
  Log.print("   y = "); Log.print(relPositionY);
  Log.print("  heading from start = "); Log.println(headingFromStart);
#endif
      
  Log.print("Roll raw value: "); Log.print(robosailRollAccel);
  Log.print(", Roll in deg: "); Log.print(robosailRoll);
  Log.print(", Actual heading: "); Log.println(heading);
  Log.println();
}

