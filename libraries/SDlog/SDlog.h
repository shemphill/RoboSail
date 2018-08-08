#ifndef _SDLOG_H_
#define _SDLOG_H_

#include <SPI.h>
#include <SD.h>

class SDlog {
public:
  SDlog(int cs = PIN_SPI_SS) { chipSelect = cs; }
  operator bool();	// in case client wants to know if we're logging
  template<typename... Ts> size_t print(Ts...);
  template<typename... Ts> size_t println(Ts...);
private:
  int chipSelect;
  bool failed = false;  // true if we couldn't open log for output
  bool inited = false;  // true if we tried to open log for output
  bool virgin = true;   // true if we are at the beginning of a line
  static const char *const dirName = "logs";
  static const char *const baseName = "logs/log0000.txt";
  char logName[strlen(baseName)+1];
  void init();
  File logFile;
};  

inline SDlog::operator bool() {
  if (failed) return false;
  if (inited) return true;
  init();
  return !failed;
}

template<typename... Ts>
size_t SDlog::print(Ts... args)
{
  if (failed) return 0;
  if (!inited) { init(); if (failed) return 0; }
  if (virgin) {
    virgin = false;
    logFile.print(millis());
    logFile.print(", ");
  }
  return logFile.print(args...);
}

template<typename... Ts>
size_t SDlog::println(Ts... args)
{
  if (failed) return 0;
  if (!inited) { init(); if (failed) return 0; }
  if (virgin) {
    logFile.print(millis());
    logFile.print(", ");
  }
  size_t retval = logFile.println(args...);
  logFile.flush();
  virgin = true;
  return retval;
}

extern SDlog Log;

#endif
