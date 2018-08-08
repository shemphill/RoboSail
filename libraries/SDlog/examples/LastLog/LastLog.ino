#include <SPI.h>
#include <SD.h>

const int chipSelect = PIN_SPI_SS;

void setup() {
  const char *const baseName = "logs/log0000.txt";
  char *logName = NULL;
  char testName[strlen(baseName)+1];

  Serial.begin(115200);
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");

  strcpy(testName, baseName);
  while (SD.exists(testName)) {
    if (logName) strcpy(logName, testName);
    else logName = strdup(testName);
    int i;
    for (i = 11; i >= 8; i--) {
      if (testName[i] < '9') {
	testName[i]++;
	break;
      }
      testName[i] = '0';
    }
    if (i < 8) break;
  }
  if (!logName) {
    Serial.println("no log file found");
    return;
  }
  Serial.print("dumping log file: ");
  Serial.println(logName);
  Serial.println("----------------------------------");

  File logFile = SD.open(logName);
  if (!logFile) {
    Serial.println("error opening log file");
    return;
  }

  while (logFile.available()) Serial.write(logFile.read());

  logFile.close();
}

void loop() {
}

