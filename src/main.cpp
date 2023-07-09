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
#include "Measurement.h"

WiFiClient client;

MyDateTime dt;

// view model
Measurement temperature("Temperature", "[deg]");
Measurement humidity("Humidity", "[%%]");
Measurement ccpm("CO2", "[ppm]");

// device
SHT3X sht30;
MHZ19C mhz19c;

void acquisitionTask(void* arg)
{
  Wire.begin();
  mhz19c = MHZ19C(36);

  while (1)
  {
    if(mhz19c.get() == 0)
    {
      ccpm.SetValue(mhz19c.ccpm);
    }

    if(sht30.get() == 0)
    {
      temperature.SetValue(sht30.cTemp);
      humidity.SetValue(sht30.humidity);
    }

    vTaskDelay(100);
  }
}

void setup()
{
	// put your setup code here, to run once:
	M5.begin(115200);
	M5.Lcd.setTextSize(3);
	M5.Lcd.setBrightness(64);
	disableCore0WDT();
	disableCore1WDT();


	// M5.Lcd.sleep();
	// M5.Lcd.setBrightness(0);
	// Serial.println("Hello World!");

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

  xTaskCreatePinnedToCore(acquisitionTask, "Task0", 4096, NULL, 1, NULL, 1);
}

void loop()
{
  dt.GetLocalTime();

  M5.Lcd.setCursor(0, 0);

  M5.Lcd.printf("%02d:%02d:%02d\r\n" ,dt.dt_hour ,dt.dt_min ,dt.dt_sec);

  M5.Lcd.printf("\r\n");

  M5.Lcd.printf("%s:\r\n", ccpm.GetTitle());
  M5.Lcd.printf("%4.2f %s\r\n", ccpm.GetValue(), ccpm.GetUnit());
  M5.Lcd.printf("\r\n");

  M5.Lcd.printf("%s:\r\n", temperature.GetTitle());
  M5.Lcd.printf("%2.1f %s\r\n", temperature.GetValue(), temperature.GetUnit());
  M5.Lcd.printf("\r\n");

  M5.Lcd.printf("%s:\r\n", humidity.GetTitle());
  M5.Lcd.printf("%2.1f %s\r\n", humidity.GetValue(), humidity.GetUnit());

	vTaskDelay(200);
}


