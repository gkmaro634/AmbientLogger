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
#define ROW0_Y      ROW_MARGIN
#define ROW1_Y      HEADER_HEIGHT+ROW_MARGIN*2
#define ROW2_Y      HEADER_HEIGHT+BLOCK_HEIGHT+ROW_MARGIN*2
#define COL_MARGIN  8
#define COL0_X      COL_MARGIN
#define COL1_X      BLOCK_WIDTH+COL_MARGIN

#define HEADER_HEIGHT   48
#define BLOCK_HEIGHT    (80+ROW_MARGIN)
#define LABEL_HEIGHT    26
#define CHART_HEIGHT   BLOCK_HEIGHT*2-ROW_MARGIN
#define HEADER_WIDTH    320
#define BLOCK_WIDTH     160
#define CHART_WIDTH    320

#define UART_BAUDRATE   115200

#define WIFI_CONNECT_TIMEOUT_MS    10000
#define ACQ_INTERVAL_MS     5000
#define PRINT_INTERVAL_MS   500

#define MHZ19C_PWM_PIN  36

// Function Prototypes
void connectWifiTask(void* arg);
void checkWifiStateTask(void* arg);
void acquisitionTask(void* arg);
void printTask(void* arg);

// typedef
typedef enum {
    INDICATOR = 1,
    WAVE_CHART,
} ModeType;

#endif /* MAIN_H */