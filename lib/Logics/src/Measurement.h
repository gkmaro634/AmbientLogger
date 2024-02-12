#ifndef MEASUREMENT_H
#define MEASUREMENT_H

class Measurement {
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
  // struct tm _timeinfo;
};

#endif /* MEASUREMENT_H */