#include <Arduino.h>
#include <M5Core2.h>
#include <Wire.h>
#include <WiFi.h>
#include <math.h>
#include "secret.h"
#include "SHT3X.h"
#include "MHZ19C.h"
#include "MyDateTime.h"
#include "time.h"

WiFiClient client;

SHT3X sht30;
float temperature = 0.0;
float humidity = 0.0;

hw_timer_t * timer0 = NULL;
portMUX_TYPE timer0Mutex = portMUX_INITIALIZER_UNLOCKED;
volatile bool isTimer0Ticked = false;
const uint16_t timer0Prescaler = 80;// 80MHz / 80 = 1MHz
const int esp32SystemClock = 80 * 1e6;
const int timer0TickIntervalMilliSeconds = 30 * 1000;
uint64_t timer0InterruptTick =  (double)esp32SystemClock / (double)timer0Prescaler * (double)timer0TickIntervalMilliSeconds / 1000.0;

float ccpm = 0.0;
MHZ19C mhz19c;

MyDateTime dt;

void IRAM_ATTR onTimer0Ticked(){
	portENTER_CRITICAL_ISR(&timer0Mutex);
	isTimer0Ticked = true;
	portEXIT_CRITICAL_ISR(&timer0Mutex);
}

void setup()
{
	// put your setup code here, to run once:
	M5.begin(115200);
	M5.Lcd.setTextSize(3);
	M5.Lcd.setBrightness(64);
	disableCore0WDT();
	disableCore1WDT();

  Wire.begin();

	// M5.Lcd.sleep();
	// M5.Lcd.setBrightness(0);
	// Serial.println("Hello World!");

  mhz19c = MHZ19C(36);

	timer0 = timerBegin(0, timer0Prescaler, true);// 1us?
	timerAttachInterrupt(timer0, &onTimer0Ticked, true);
	timerAlarmWrite(timer0, timer0InterruptTick, true);

	WiFi.begin(ssid, pass);
	while( WiFi.status() != WL_CONNECTED)
  {
		// TODO: timeout
		delay(500);
		M5.Lcd.print(".");
    	// Serial.println(".");
	}
	M5.Lcd.println("WiFi connected");
	// Serial.println("WiFi connected");

  dt.Initialize();

	delay(1000);
	M5.Lcd.clear();

	timerAlarmEnable(timer0);
}

bool isSetValue = false;
bool toUpdateDisplay = false;

void loop()
{
	portENTER_CRITICAL_ISR(&timer0Mutex);
	if(isTimer0Ticked == true)
  {
		isSetValue = true;
		isTimer0Ticked = false;
	}
	portEXIT_CRITICAL_ISR(&timer0Mutex);

	if(isSetValue == true){
		isSetValue = false;
	}

  if(mhz19c.get() != 0)
  {
    toUpdateDisplay = false;
    ccpm = 0.0;
  }
  else
  {
    toUpdateDisplay = true;
    ccpm = mhz19c.ccpm;
  }

  if(sht30.get() == 0)
  {
    temperature = sht30.cTemp;
    humidity = sht30.humidity;
  }

  dt.GetLocalTime();

  // display current values
  if(toUpdateDisplay == true)
  {
    M5.Lcd.setCursor(0, 0);
    // M5.Lcd.println("Failed to obtain time");
    M5.Lcd.printf("%02d:%02d:%02d\r\n" ,dt.dt_hour ,dt.dt_min ,dt.dt_sec);

    M5.Lcd.printf("\r\n");

    M5.Lcd.printf("CO2:\r\n");
    M5.Lcd.printf("%4.2f [ppm]\r\n", ccpm);
    M5.Lcd.printf("\r\n");

    M5.Lcd.printf("Temperature:\r\n");
    M5.Lcd.printf("%2.1f [deg]\r\n", temperature);
    M5.Lcd.printf("\r\n");

    M5.Lcd.printf("Humidity:\r\n");
    M5.Lcd.printf("%2.1f [%%]\r\n", humidity);
  }

	delay(1);
}


