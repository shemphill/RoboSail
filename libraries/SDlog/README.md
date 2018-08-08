# Scott Hemphill's SDlog support for the Adafruit MicroSD Breakout Board

This library file allows the automatic creation of log files on a MicroSD
card.

## How to use the library ##

Simply add the line:

```
#include "SDlog.h"
```

in your project file, and it will declare a variable named "Log".  You can
use the functions "Log.print()" and "Log.println()" in exactly the same ways
that you would normally use "Serial.print()" and "Serial.println()".  When
the Arduino is restarted, the library will automatically create a subdirectory
named "LOG" in the top level directory of the SD card if it doesn't already
exist.  Then it will create a log file in that subdirectory named "LOG0000.TXT"
or "LOG0001.TXT", etc.  It will create a new log file every time the Arduino
is restarted, incrementing the number in the name of the last log file to
get the name of the new log file.

In addition to the data printed to the log by your project, the current time
in milliseconds (from "millis()") is prepended to the beginning of each line.
The library understands that it is at the beginning of a line when it is first
initialized, and then after every call to "Log.println()".  Therefore, if
your code ever uses something like

```
  Log.print("\n");
```

the library will not understand that it is now at the beginning of a line,
and the next line will *not* be prepended with the time in milliseconds.
So you should use "Log.println()" instead.

Note: the include file "SDlog.h" defines the variable "Log".  If you
have a shield that uses a chip select pin which differs from the default,
you can replace that line near the end of the file with one that looks like:

```
  SDlog Log(4);  // use pin 4 for chip select
```


## How to hook up the Adafruit MicroSD Breakout Board to the Arduino Mega ##

These instructions are for the Arduino Mega with the Grove Mega Shield
installed.

I use a 2x2 Dupont connector to pins 50-53 on the Mega.  These pins are
on the Mega itself, not on the shield, but they are exposed because the
shield is slightly shorter than the Mega board.

pin 50  ->  DO     (I use an orange wire)
pin 51  ->  DI     (I use a yellow wire)
pin 52  ->  CLK    (I use a green wire)
pin 53  ->  CS     (I use a blue wire)

I also use a Grove plug for power and ground

G (black wire)  ->  GND
V (red wire)    ->  5v

This Grove plug can be inserted into any of the unused sockets on the
shield.  I use the A14-A15 socket, because it is close to pins 50-53.

## How to hook up the Adafruit to the Arduino Uno

I haven't connected to the Uno myself, but you can connect the signals
to pins 10-13 as follows:

pin 10  ->  CS
pin 11  ->  DI
pin 12  ->  DO
pin 13  ->  CLK

And, of course, you will also need to wire up power and ground.

## Using an ICSP header

The ICSP header can be used, with pins connected as follows:

MISO  ->  DO
VCC   ->  5v
SCK   ->  CLK
MOSI  ->  DI
RESET ->  (not connected)
GND   ->  GND
