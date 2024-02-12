#ifndef MHZ19C_H
#define MHZ19C_H

#include <Arduino.h>
#include <M5Unified.h>

class MHZ19C {
public:
  // Constructor
  MHZ19C(int pwmPin = 36);

  // Public methods
  byte get(void);

  // Public variables
  float ccpm; // CO2 concentration per million
  // float cTemp = 0; // Celsius temperature
  // float fTemp = 0; // Fahrenheit temperature
  // float humidity = 0; // Humidity

private:
  // Private members
  int _pwmPin; // PWM pin number
  unsigned int _outputHighDuration; // Duration of output high signal
  // uint8_t _address; // Address (if needed)
};

#endif /* MHZ19C_H */