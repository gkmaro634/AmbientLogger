#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <M5Unified.h>

class Measurement {
public:
  Measurement(const char* title, const char* unit);
  int Enqueue(float sendValue);
  int Dequeue(float *receiveValue, int timeoutMs);
  int Peek(float *receiveValue, int timeoutMs);
  const char* GetTitle();
  const char* GetUnit();

private:
  const char* _title;
  const char* _unit;
  float _value = 0.0;
  QueueHandle_t _xQueue;

  // struct tm _timeinfo;
};

#endif /* MEASUREMENT_H */
