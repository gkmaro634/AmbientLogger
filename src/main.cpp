#include "main.h"

// valiables
WiFiClient client;

MyDateTime dt;
M5GFX display;
M5Canvas canvas(&display);

Measurement temperature("Temp.", "C");
Measurement humidity("Hum.", "%");
Measurement ccpm("CO2", "ppm");
DiscomfortIndex discomfortIndex;

SHT3X sht30;
MHZ19C mhz19c;

TimerHandle_t acqTimer;

void setup()
{
	// put your setup code here, to run once:
	M5.begin();
  Serial.begin(115200);
  display.init();
  // display.setTextFont(2);// 16px ascii
	// display.setTextSize(3);
	display.setBrightness(64);
  canvas.createSprite(display.width(), display.height());

	disableCore0WDT();
	disableCore1WDT();

	// display.sleep();
	// display.setBrightness(0);
	// Serial.println("Hello World!");

	WiFi.begin(ssid, pass);
	while( WiFi.status() != WL_CONNECTED)
  {
		// TODO: timeout
		delay(500);
		display.print(".");
    	// Serial.println(".");
	}
	display.println("WiFi connected");
	// Serial.println("WiFi connected");

  dt.Initialize();

	delay(1000);
	display.clear();

  Wire.begin();
  mhz19c = MHZ19C(36);

  acqTimer = xTimerCreate("acqTask", pdMS_TO_TICKS(5000), pdTRUE, NULL, acquisitionTask);
  xTimerStart(acqTimer, 0);
  // xTaskCreatePinnedToCore(acquisitionTask, "Task0", 4096, NULL, 1, NULL, 1);
}

void loop()
{
  dt.GetLocalTime();

  canvas.fillScreen(BLACK);

  // grid row0,col0
  canvas.setTextFont(7);// 48px 7seg
  canvas.setCursor(COL0_X, ROW0_Y);
	canvas.setTextSize(1);
  canvas.printf("%02d:%02d\r\n" ,dt.dt_hour ,dt.dt_min);

  // canvas.setTextFont(7);// 48px 7seg
  // canvas.setCursor(0, 0);
	// canvas.setTextSize(1);
  // canvas.printf("%02d/%02d\r\n" ,dt.dt_month ,dt.dt_day);

  // grid row1,col0
  canvas.setTextFont(4);// 26px ascii
  canvas.setCursor(COL0_X, ROW1_Y + ROW_MARGIN);
	canvas.setTextSize(1);
  canvas.printf("%s [%s]:\r\n", ccpm.GetTitle(), ccpm.GetUnit());

  canvas.setTextFont(7);// 48px 7seg
  canvas.setCursor(COL0_X, ROW1_Y + 26 + ROW_MARGIN);
	canvas.setTextSize(1);
  canvas.printf("%4.0f\r\n", ccpm.GetValue());

  // grid row1.col1
  canvas.setTextFont(4);// 26px ascii
  canvas.setCursor(COL1_X, ROW1_Y + ROW_MARGIN);
	canvas.setTextSize(1);
  if (discomfortIndex.GetValue() < 55)
  {
    canvas.printf("((>_<))  \r\n");
  }
  else if(discomfortIndex.GetValue() < 75)
  {
    canvas.printf("(^_^)    \r\n");
  }
  else
  {
    canvas.printf("(x_x;)   \r\n");
  }

  canvas.setTextFont(7);// 48px 7seg
  canvas.setCursor(COL1_X, ROW1_Y + 26 + ROW_MARGIN);
	canvas.setTextSize(1);
  canvas.printf("%d\r\n", discomfortIndex.GetValue());

  // grid row2,col0
  canvas.setTextFont(4);// 26px ascii
  canvas.setCursor(COL0_X, ROW2_Y + ROW_MARGIN);
	canvas.setTextSize(1);
  canvas.printf("%s [%s]:\r\n", temperature.GetTitle(), temperature.GetUnit());

  canvas.setTextFont(7);// 48px 7seg
  canvas.setCursor(COL0_X, ROW2_Y + 26 + ROW_MARGIN);
	canvas.setTextSize(1);
  canvas.printf("%2.1f\r\n", temperature.GetValue());

  // grid row2,col1
  canvas.setTextFont(4);// 26px ascii
  canvas.setCursor(COL1_X, ROW2_Y + ROW_MARGIN);
	canvas.setTextSize(1);
  canvas.printf("%s [%s]:\r\n", humidity.GetTitle(), humidity.GetUnit());

  canvas.setTextFont(7);// 48px 7seg
  canvas.setCursor(COL1_X, ROW2_Y + 26 + ROW_MARGIN);
	canvas.setTextSize(1);
  canvas.printf("%2.1f\r\n", humidity.GetValue());
  
  display.startWrite(); 
  canvas.pushSprite(0, 0);
  display.endWrite(); 

	vTaskDelay(200);
}

void acquisitionTask(void* arg)
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
}


