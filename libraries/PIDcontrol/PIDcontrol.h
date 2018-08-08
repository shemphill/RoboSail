#ifndef _PIDCONTROL_H_
#define _PIDCONTROL_H_

template <typename T>
class PIDcontrol
{
public:
  PIDcontrol(T Kp, T Ki = 0);
  void reset();
  T output(T);
  void setKp(T t) { Kp = t; }
  void setKi(T t) { Ki = t; }
  void setIntMin(T t) { intMin = t; }
  void setIntMax(T t) { intMax = t; }
private:
  T Kp, Ki;
  unsigned long previous_time;
  unsigned long now;
  T previous_error;
  T error;
  T integral;
  T intMin;
  T intMax;
};

template <typename T>
PIDcontrol<T>::PIDcontrol(T Kp, T Ki)
{
  this->Kp = Kp;
  this->Ki = Ki;
}

template <typename T>
T PIDcontrol<T>::output(T err)
{
  unsigned long dt;
  previous_time = now;
  now = millis();
  dt = now - previous_time;
  previous_error = error;
  error = err;
  integral += error * dt;
  if (integral < intMin) integral = intMin;
  if (integral > intMax) integral = intMax;
  return Kp*error + Ki*integral;
}

template <typename T>
void PIDcontrol<T>::reset()
{
  now = millis();
  error = 0;
  integral = 0;
}

#endif
