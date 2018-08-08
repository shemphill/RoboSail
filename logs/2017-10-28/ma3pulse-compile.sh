#!/bin/bash

CC="/home/hemphill/arduino-1.8.4/hardware/tools/avr/bin/avr-gcc"
CFLAGS="-c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects -mmcu=atmega2560"
CFLAGS="-c -g -O2 -w -std=gnu11 -ffunction-sections -fdata-sections -mmcu=atmega2560"
DEFINES="-DF_CPU=16000000L -DARDUINO=10804 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR"
INCLUDES="-I/home/hemphill/arduino-1.8.4/hardware/arduino/avr/cores/arduino -I/home/hemphill/arduino-1.8.4/hardware/arduino/avr/variants/mega"

$CC $CFLAGS $DEFINES $INCLUDES -gstabs -Wa,-ahlmsd=output.lst -dp -fverbose-asm ma3pulse.c
