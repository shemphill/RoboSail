

void getCommand()
{
  const bool logCommands = true;
  const int CMD_BITS = 4;
  const int MIDDLE = -1, LEFT = 0, RIGHT = 1;
  static unsigned long begin, end, lastBitTime, now;
  static int state=-1, oldstate=-1, bits=0, bitcount=0;
  now = millis();
  if (rudderPulseWidth < RUDDER_LOW+50) state = LEFT;
  else if (rudderPulseWidth > RUDDER_HIGH-50) state = RIGHT;
  else if (RUDDER_MIDDLE-50 < rudderPulseWidth &&
	   rudderPulseWidth < RUDDER_MIDDLE+50) state = MIDDLE;
  else return;  // some other place, could just be noise
  if (state != oldstate) {
    if (oldstate >= 0 &&
	end-begin > 100) {  // length of pulse at least 0.1 sec
      // new pulse starting, shift in latest bit of command
      if (logCommands) {
	Log.print("@registering a ");
	Log.print(oldstate);
	Log.println(" bit");
      }
      if (now - lastBitTime < 1000) {  // was last bit within 1 sec ?
	bits = (bits << 1) | oldstate; // yes, shift in new bit
	bitcount++;
      } else {                         // bit isolated in time, start over
	bits = oldstate;
	bitcount = 1;
      }
      lastBitTime = now;
    }
    oldstate = state;
    begin = now;
  }
  end = now;
  if (bitcount == CMD_BITS) { // have we collected a complete command?
    if (command != bits) {    // if new command, save the previous one
      lastcommand = command;
      command = bits;
    }
    bits = 0;
    bitcount = 0;
    if (logCommands) {
      Log.print("@command ");
      Log.print(command);
      Log.println(" registered");
    }
  }
  if (sailPulseWidth < SAIL_HIGH - 50) {
    if (!manualOverride) {
      manualOverride = true;
      if (logCommands) Log.println("@manual command mode");
    }
  } else if (manualOverride) { 
    manualOverride = false;
    if (logCommands) {
      Log.print("@returning to automatic command ");
      Log.println(command);
    }
  }
}
