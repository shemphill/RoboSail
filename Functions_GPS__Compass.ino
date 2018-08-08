#ifdef USE_GPS

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  GPS.read(); // reads char (if available) into internal buffer in GPS object
}

// function to enable TIMER0 interrupt for GPS
void enableInterrupt() {
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

void readGPS()  //gets GPS data, parses it, and returns (x,y) position in meters in array called pos[]
{
if (GPS.newNMEAreceived())
  {
    char* LastNMEA; // declare pointer to GPS data
    LastNMEA = GPS.lastNMEA(); // read the string and set the newNMEAreceived() flag to false
    if (!GPS.parse(LastNMEA)) 
    {
      return; // failed to parse a sentence (was likely incomplete) so just wait for another
    }
//    Serial.println("\nNew data from GPS");
//    GPSfix = GPS.fix;  //put parsed data in variables for printing
//    GPSqual = GPS.fixquality;
//    GPSsat = GPS.satellites;
    if (GPS.fix)
    {
      calc.GPStoXY(GPS, relPositionX, relPositionY);
      angleFromStart = atan2(relPositionY, relPositionX) * RAD_TO_DEG;
      angleFromStart = (angleFromStart + 360) % 360;
      headingFromStart = atan2(relPositionX, relPositionY) * RAD_TO_DEG + 360.5;
      headingFromStart %= 360;
    }
  }
}
#endif

void readCompassAccel()  //reads Compass to get heading and tilt
{
  float ax, ay, az, mx, my, mz;
  float my_adj, mx_adj;

   /* Get a new sensor event */
  sensors_event_t accel_event;
  accel.getEvent(&accel_event);
  sensors_event_t mag_event;
  mag.getEvent(&mag_event);

  /* Invert X so that when when X, Y, or Z is pointed down, it has a positive reading. */
  ax = -accel_event.acceleration.x;
  ay = accel_event.acceleration.y;
  az = accel_event.acceleration.z;
  
  /* Adjust for hard iron effects */
  mx = mag_event.magnetic.x - hardiron_x;
  my = mag_event.magnetic.y - hardiron_y;
  mz = mag_event.magnetic.z - hardiron_z;

  /* Invert Y and Z axis so that when X, Y, or Z is pointed towards Magnetic North they get a positive reading. */
  my = -my;
  mz = -mz;
  
  roll = atan2(ay,az);
  pitch = atan(-ax/sqrt(pow(ay,2)+pow(az,2)));
  
  my_adj = mz*sin(roll) - my*cos(roll);
  mx_adj = (mx*cos(pitch) + my*sin(pitch)*sin(roll) + mz*sin(pitch)*cos(roll));
  
  yaw = atan2(my_adj,mx_adj);
  
  roll *= RAD_TO_DEG;
  pitch *= RAD_TO_DEG;
  yaw *= RAD_TO_DEG;
  
  heading = yaw + declination + 360.5;
  heading %= 360;

 //The heading is converted to a frame of reference for RoboSail:
 // East is 0 degrees, North is 90 deg, West is 180 deg, South is 270 deg. 
  robosailHeading = (450 - heading) % 360;
  
  //define roll for RoboSail as rolling to Port side is positive, rolling to Starboard is negative
  robosailRoll  = -1 * roll; 
}
