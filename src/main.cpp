#include "main.h"

// valiables
WiFiClient client;

MyDateTime myDateTime;
M5GFX display;
M5Canvas canvas(&display);

Measurement temperature("Temp.", "C");
Measurement humidity("Hum.", "%");
Measurement ccpm("CO2", "ppm");
DiscomfortIndex discomfortIndex;

SHT3X sht30;
MHZ19C mhz19c;

TaskHandle_t handleWifiConnectTask;

TimerHandle_t handleCheckWifiStateTimer;
TimerHandle_t handleAcqTimer;
TimerHandle_t handlePrintTimer;

void setup()
{
	// put your setup code here, to run once:
  BaseType_t status;

  // デバイス初期化
	M5.begin();
  Serial.begin(UART_BAUDRATE);
  Wire.begin();
  mhz19c = MHZ19C(MHZ19C_PWM_PIN);

  // 画面初期化
  display.init();
	display.setBrightness(64);
  canvas.createSprite(display.width(), display.height());

	// Task初期化
  disableCore0WDT();
	disableCore1WDT();

  status = xTaskCreatePinnedToCore(connectWifiTask, "wifiTask", 4096, NULL, 1, &handleWifiConnectTask, 1);
  configASSERT(status == pdPASS);

  handleCheckWifiStateTimer = xTimerCreate("checkWifiStateTask", pdMS_TO_TICKS(500), pdTRUE, NULL, checkWifiStateTask);
  handleAcqTimer = xTimerCreate("acqTask", pdMS_TO_TICKS(ACQ_INTERVAL_MS), pdTRUE, NULL, acquisitionTask);
  handlePrintTimer = xTimerCreate("printTask", pdMS_TO_TICKS(PRINT_INTERVAL_MS), pdTRUE, NULL, printTask);
}

void loop()
{
  vTaskDelete(NULL);
}

void connectWifiTask(void* arg){
  // 接続開始
	WiFi.begin(ssid, pass);
  display.println("Start WiFi connection");

  // 状態監視開始
  xTimerStart(handleCheckWifiStateTimer, 0);

  // 通知を待つ
  if (xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(WIFI_CONNECT_TIMEOUT_MS))){
    // 接続完了の場合
    myDateTime.Initialize();
    display.println("WiFi connected");
  }
  else{
    // タイムアウトの場合
    display.println("WiFi connection timed out");
  }

  xTimerStop(handleCheckWifiStateTimer, 0);

  // メイン処理初回実行
  acquisitionTask(NULL);
  printTask(NULL);

  // メイン処理タイマ開始
  xTimerStart(handleAcqTimer, 0);
  xTimerStart(handlePrintTimer, 0);

  vTaskDelete(NULL);
}

void checkWifiStateTask(void* arg){
  // print "."
	display.print(".");

  // 状態を取得し接続完了なら通知する
  if (WiFi.status() == WL_CONNECTED){
    // 通知
    xTaskNotifyGive(handleWifiConnectTask);
  }
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

void printTask(void *arg){
  myDateTime.GetLocalTime();

  canvas.fillScreen(BLACK);

  // grid row0,col0
  canvas.setTextFont(7);// 48px 7seg
  canvas.setCursor(COL0_X, ROW0_Y);
	canvas.setTextSize(1);
  canvas.printf("%02d:%02d\r\n" ,myDateTime.dt_hour ,myDateTime.dt_min);

  // canvas.setTextFont(7);// 48px 7seg
  // canvas.setCursor(0, 0);
	// canvas.setTextSize(1);
  // canvas.printf("%02d/%02d\r\n" ,myDateTime.dt_month ,myDateTime.dt_day);

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
}
