#ifndef SHT3X_H
#define SHT3X_H

#include <Arduino.h>
#include <Wire.h>

class SHT3X {
public:
  // Constructor
  SHT3X(uint8_t address = 0x44);

  // Public methods
  byte get(void);

  // Public variables
  float cTemp = 0;
  float fTemp = 0;
  float humidity = 0;

private:
  // Private members
  uint8_t _address;
};

#endif /* SHT3X_H */