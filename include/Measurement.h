#ifndef __MEASUREMENT_H
#define __MEASUREMENT_H

class Measurement{
public:
  Measurement(const char* title, const char* unit);
  void SetValue(float v);
  const char* GetTitle();
  const char* GetUnit();
  float GetValue();

private:
  const char* _title;
  const char* _unit;
  float _value = 0.0;

//   struct tm _timeinfo;
};

#endif