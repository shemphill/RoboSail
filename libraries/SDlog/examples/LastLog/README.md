# Scott Hemphill's example program to read the most recent log file

This example program reads the most recent log file created by the SDlog
library, and displays it on the Serial monitor.

## How to use the program ##

Just upload it to the Arduino and run it.  It will attempt to find files
named "LOG/LOG0000.TXT" or "LOG/LOG0001.TXT", etc., on the SD card,
adding one to number in the file name until it fails to find a file
with that name.  It then opens, reads, and displays the last log file
that it had successfully found.
