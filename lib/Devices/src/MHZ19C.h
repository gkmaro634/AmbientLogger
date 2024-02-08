#ifndef __MHZ19C_H
#define __MHZ19C_H

// #include "Wire.h"
#include <Arduino.h>
#include <M5Unified.h>

class MHZ19C{
public:
  MHZ19C(int pwmPin=36);
  byte get(void);
  float ccpm;
  // float cTemp=0;
  // float fTemp=0;
  // float humidity=0;

private:
  int _pwmPin;
  unsigned int _outputHighDuration;
  // uint8_t _address;
};


#endif