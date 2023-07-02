#include <Arduino.h>
#include <M5Core2.h>
#include <Wire.h>
#include <WiFi.h>
#include <math.h>
#include "secret.h"
#include "SHT3X.h"
#include "time.h"

WiFiClient client;

SHT3X sht30;
float temperature = 0.0;
float humidity = 0.0;

const char* ntpServer = "ntp.nict.jp";
const long  gmtOffset_sec = 3600 * 9;
const int   daylightOffset_sec = 0;

hw_timer_t * timer0 = NULL;
portMUX_TYPE timer0Mutex = portMUX_INITIALIZER_UNLOCKED;
volatile bool isTimer0Ticked = false;
const uint16_t timer0Prescaler = 80;// 80MHz / 80 = 1MHz
const int esp32SystemClock = 80 * 1e6;
const int timer0TickIntervalMilliSeconds = 30 * 1000;
uint64_t timer0InterruptTick =  (double)esp32SystemClock / (double)timer0Prescaler * (double)timer0TickIntervalMilliSeconds / 1000.0;

const int mhz19cPin = 36;
unsigned int mhz19cOutputHighDuration = 0;
float cppm = 0.0;

void IRAM_ATTR onTimer0Ticked(){
	portENTER_CRITICAL_ISR(&timer0Mutex);
	isTimer0Ticked = true;
	portEXIT_CRITICAL_ISR(&timer0Mutex);
}

void printLocalTime()
{
  struct tm timeinfo;

  if(!getLocalTime(&timeinfo)){
    // M5.Lcd.println("Failed to obtain time");
    return;
  }

  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(40,100);
  M5.Lcd.printf("%04d-%02d-%02d %02d:%02d:%02d" 
                ,timeinfo.tm_year + 1900
                ,timeinfo.tm_mon
                ,timeinfo.tm_mday
                ,timeinfo.tm_hour
                ,timeinfo.tm_min
                ,timeinfo.tm_sec
                );
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

	pinMode(mhz19cPin, INPUT);
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

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  

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

	while(true)
  {
		if(digitalRead(mhz19cPin) != LOW)
    {
			break;
		}
	}

	mhz19cOutputHighDuration = pulseIn(mhz19cPin, HIGH, 3000 * 1000);// TODO: other thread
	if(mhz19cOutputHighDuration != 0)
  {
		float th = (float)mhz19cOutputHighDuration / 1000.0;
		cppm = 2.0 * (th - 2.0);
    toUpdateDisplay = true;
  }
  else
  {
    toUpdateDisplay = false;
  }

  if(sht30.get() == 0)
  {
    temperature = sht30.cTemp;
    humidity = sht30.humidity;
  }

  // display current values
  if(toUpdateDisplay == true)
  {
    M5.Lcd.setCursor(0, 0);

    struct tm timeinfo;
    if(getLocalTime(&timeinfo))
    {
      // M5.Lcd.println("Failed to obtain time");
      M5.Lcd.printf("%02d:%02d:%02d\r\n"
                    ,timeinfo.tm_hour
                    ,timeinfo.tm_min
                    ,timeinfo.tm_sec
                    );
      M5.Lcd.printf("\r\n");
    }
    else
    {
      M5.Lcd.printf("**:**:**");
    }

    M5.Lcd.printf("CO2:\r\n");
    M5.Lcd.printf("%4.2f [ppm]\r\n", cppm);
    M5.Lcd.printf("\r\n");
    M5.Lcd.printf("Temperature:\r\n");
    M5.Lcd.printf("%2.1f [deg]\r\n", temperature);
    M5.Lcd.printf("\r\n");
    M5.Lcd.printf("Humidity:\r\n");
    M5.Lcd.printf("%2.1f [%%]\r\n", humidity);
  }

	delay(1);
}


