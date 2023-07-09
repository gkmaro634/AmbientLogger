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
Measurement temperature("Temp.", "C");
Measurement humidity("Hum.", "%");
Measurement ccpm("CO2", "ppm");

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
  M5.Lcd.setTextFont(2);// 16px ascii
	// M5.Lcd.setTextSize(3);
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

  // header
  M5.Lcd.setCursor(0, 0);
	M5.Lcd.setTextSize(2);// 32px
  M5.Lcd.printf("%02d/%02d\r\n" ,dt.dt_month ,dt.dt_day);

  M5.Lcd.setCursor(160, 0);
	M5.Lcd.setTextSize(2);// 32px
  M5.Lcd.printf("%02d:%02d:%02d\r\n" ,dt.dt_hour ,dt.dt_min ,dt.dt_sec);

  // grid row0,col0
  M5.Lcd.setCursor(0, 40);
	M5.Lcd.setTextSize(2);// 32px
  M5.Lcd.printf("%s [%s]:\r\n", ccpm.GetTitle(), ccpm.GetUnit());

  M5.Lcd.setCursor(0, 72);
	M5.Lcd.setTextSize(3);// 48px
  M5.Lcd.printf("%4.2f\r\n", ccpm.GetValue());

  // grid row1,col0
  M5.Lcd.setCursor(0, 136);
	M5.Lcd.setTextSize(2);// 32px
  M5.Lcd.printf("%s [%s]:\r\n", temperature.GetTitle(), temperature.GetUnit());

  M5.Lcd.setCursor(0, 168);
	M5.Lcd.setTextSize(3);// 48px
  M5.Lcd.printf("%2.1f\r\n", temperature.GetValue());

  // grid row1,col1
  M5.Lcd.setCursor(160, 136);
	M5.Lcd.setTextSize(2);// 32px
  M5.Lcd.printf("%s [%s]:\r\n", humidity.GetTitle(), humidity.GetUnit());

  M5.Lcd.setCursor(160, 168);
	M5.Lcd.setTextSize(3);// 48px
  M5.Lcd.printf("%2.1f\r\n", humidity.GetValue());

	vTaskDelay(200);
}


