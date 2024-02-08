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
#include "DiscomfortIndex.h"

#define ROW_MARGIN  8
#define ROW0_Y      0
#define ROW1_Y      48
#define ROW2_Y      144
#define COL0_X      0
#define COL1_X      160

WiFiClient client;

MyDateTime dt;

TFT_eSprite sprite(&M5.Lcd);

// view model
Measurement temperature("Temp.", "C");
Measurement humidity("Hum.", "%");
Measurement ccpm("CO2", "ppm");
DiscomfortIndex discomfortIndex;

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
      discomfortIndex.Update(temperature.GetValue(), humidity.GetValue());
    }

    vTaskDelay(100);
  }
}

void setup()
{
	// put your setup code here, to run once:
	M5.begin(115200);
  // M5.Lcd.setTextFont(2);// 16px ascii
	// M5.Lcd.setTextSize(3);
	M5.Lcd.setBrightness(64);
  sprite.createSprite(M5.Lcd.width(), M5.Lcd.height());

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

  sprite.fillScreen(BLACK);

  // grid row0,col0
  sprite.setTextFont(7);// 48px 7seg
  sprite.setCursor(COL0_X, ROW0_Y);
	sprite.setTextSize(1);
  sprite.printf("%02d:%02d\r\n" ,dt.dt_hour ,dt.dt_min);

  // sprite.setTextFont(7);// 48px 7seg
  // sprite.setCursor(0, 0);
	// sprite.setTextSize(1);
  // sprite.printf("%02d/%02d\r\n" ,dt.dt_month ,dt.dt_day);

  // grid row1,col0
  sprite.setTextFont(4);// 26px ascii
  sprite.setCursor(COL0_X, ROW1_Y + ROW_MARGIN);
	sprite.setTextSize(1);
  sprite.printf("%s [%s]:\r\n", ccpm.GetTitle(), ccpm.GetUnit());

  sprite.setTextFont(7);// 48px 7seg
  sprite.setCursor(COL0_X, ROW1_Y + 26 + ROW_MARGIN);
	sprite.setTextSize(1);
  sprite.printf("%4.0f\r\n", ccpm.GetValue());

  // grid row1.col1
  sprite.setTextFont(4);// 26px ascii
  sprite.setCursor(COL1_X, ROW1_Y + ROW_MARGIN);
	sprite.setTextSize(1);
  if (discomfortIndex.GetValue() < 55)
  {
    sprite.printf("((>_<))  \r\n");
  }
  else if(discomfortIndex.GetValue() < 75)
  {
    sprite.printf("(^_^)    \r\n");
  }
  else
  {
    sprite.printf("(x_x;)   \r\n");
  }

  sprite.setTextFont(7);// 48px 7seg
  sprite.setCursor(COL1_X, ROW1_Y + 26 + ROW_MARGIN);
	sprite.setTextSize(1);
  sprite.printf("%d\r\n", discomfortIndex.GetValue());

  // grid row2,col0
  sprite.setTextFont(4);// 26px ascii
  sprite.setCursor(COL0_X, ROW2_Y + ROW_MARGIN);
	sprite.setTextSize(1);
  sprite.printf("%s [%s]:\r\n", temperature.GetTitle(), temperature.GetUnit());

  sprite.setTextFont(7);// 48px 7seg
  sprite.setCursor(COL0_X, ROW2_Y + 26 + ROW_MARGIN);
	sprite.setTextSize(1);
  sprite.printf("%2.1f\r\n", temperature.GetValue());

  // grid row2,col1
  sprite.setTextFont(4);// 26px ascii
  sprite.setCursor(COL1_X, ROW2_Y + ROW_MARGIN);
	sprite.setTextSize(1);
  sprite.printf("%s [%s]:\r\n", humidity.GetTitle(), humidity.GetUnit());

  sprite.setTextFont(7);// 48px 7seg
  sprite.setCursor(COL1_X, ROW2_Y + 26 + ROW_MARGIN);
	sprite.setTextSize(1);
  sprite.printf("%2.1f\r\n", humidity.GetValue());

  sprite.pushSprite(0, 0);

	vTaskDelay(200);
}


