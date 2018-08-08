#ifndef _TIMEOUT_H_
#define _TIMEOUT_H_

class TimeOut {
  public:
    TimeOut(unsigned long t);
    bool operator()();
  private:
    unsigned long interval;
    unsigned long timeout;
};

TimeOut::TimeOut(unsigned long t)
{
  interval = t;
  timeout = millis() + t;
}

bool TimeOut::operator()()
{
  unsigned long now = millis();
  if (now < timeout) return false;
  timeout = (now - now%interval) + interval;
  return true;
}

#endif
