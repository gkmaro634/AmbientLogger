#ifndef MAIN_H
#define MAIN_H

// Includes
#include <Arduino.h>
#include <M5Unified.h>
#include <Wire.h>
#include <WiFi.h>
#include <math.h>
#include <time.h>
#include <M5GFX.h>
#include "secret.h"
#include "SHT3X.h"
#include "MHZ19C.h"
#include "MyDateTime.h"
#include "Measurement.h"
#include "DiscomfortIndex.h"

// Constants
#define ROW_MARGIN  8
#define ROW0_Y      0
#define ROW1_Y      48
#define ROW2_Y      144
#define COL0_X      0
#define COL1_X      160

// Function Prototypes
void acquisitionTask(void* arg);

#endif /* MAIN_H */