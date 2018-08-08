#include "SD.h"
#include "SDlog.h"

void SDlog::init()
{
  inited = true;
  failed = true;  // assume for now that card is not present

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");

// make the directory, in case it doesn't yet exist
  SD.mkdir(dirName);

// find the first log file name that doesn't exist yet
  strcpy(logName, baseName);
  while (SD.exists(logName)) {
    int i;
    for (i = 11; i >= 8; i--) {
      if (logName[i] < '9') {
	logName[i]++;
	break;
      }
      logName[i] = '0';
    }
    if (i < 8) {
      Serial.println("Log file directory full");
      return;
    }
  }

  logFile = SD.open(logName, FILE_WRITE);
  if (!logFile) {
    Serial.println("SD.open of log file failed");
    return;
  }
  failed = false;
}

SDlog Log;
