#include <Arduino.h>

unsigned int ma3pulse(uint8_t pin)
{
  volatile uint8_t *port = portInputRegister(digitalPinToPort(pin));
  uint8_t bit = digitalPinToBitMask(pin);
  unsigned int toff, ton;
  ton = *port;
  ton = *port;
  ton = 1;
  while ((*port & bit) != 0) {
    if (++ton == 1100) return 0;
  }
  toff = *port;
  toff = *port;
  toff = 1;
  while ((*port & bit) == 0) {
    if (++toff == 1100) return 0;
  }
  ton = *port;
  ton = *port;
  ton = 1;
  while ((*port & bit) != 0) {
    if (++ton == 1100) return 0;
  }
  toff = *port;
  toff = *port;
  toff = 1;
  while ((*port & bit) == 0) {
    if (++toff == 1100) return 0;
  }
  unsigned long numer = (1024L*ton-toff)*360L;
  unsigned long denom = (ton+toff)*1024L;
  unsigned int retval = (numer+denom/2)/denom;
  return retval == 360 ? 0 : retval;
}
