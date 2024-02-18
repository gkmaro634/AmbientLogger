#include "main.h"

// valiables
WiFiClient client;

MyDateTime myDateTime;
M5GFX display;
// M5Canvas canvas(&display);

M5Canvas headerCanvas(&display);
M5Canvas co2Canvas(&display);
M5Canvas disconfortCanvas(&display);
M5Canvas tempCanvas(&display);
M5Canvas humidCanvas(&display);

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
  headerCanvas.createSprite(HEADER_WIDTH, HEADER_HEIGHT);
  co2Canvas.createSprite(BLOCK_WIDTH, BLOCK_HEIGHT);
  disconfortCanvas.createSprite(BLOCK_WIDTH, BLOCK_HEIGHT);
  tempCanvas.createSprite(BLOCK_WIDTH, BLOCK_HEIGHT);
  humidCanvas.createSprite(BLOCK_WIDTH, BLOCK_HEIGHT);

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
  display.fillScreen(BLACK);
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

  // grid row0,col0
  headerCanvas.fillScreen(BLACK);
  headerCanvas.setTextFont(7);// 48px 7seg
  headerCanvas.setCursor(0, 0);
	headerCanvas.setTextSize(1);
  headerCanvas.printf("%02d:%02d\r\n" ,myDateTime.dt_hour ,myDateTime.dt_min);

  // canvas.setTextFont(7);// 48px 7seg
  // canvas.setCursor(0, 0);
	// canvas.setTextSize(1);
  // canvas.printf("%02d/%02d\r\n" ,myDateTime.dt_month ,myDateTime.dt_day);

  // grid row1,col0
  co2Canvas.fillScreen(BLACK);
  co2Canvas.setTextFont(4);// 26px ascii
  co2Canvas.setCursor(0, 0);
	co2Canvas.setTextSize(1);
  co2Canvas.printf("%s [%s]:\r\n", ccpm.GetTitle(), ccpm.GetUnit());

  co2Canvas.setTextFont(7);// 48px 7seg
  co2Canvas.setCursor(0, LABEL_HEIGHT);
	co2Canvas.setTextSize(1);
  co2Canvas.printf("%4.0f\r\n", ccpm.GetValue());

  // grid row1.col1
  disconfortCanvas.fillScreen(BLACK);
  disconfortCanvas.setTextFont(4);// 26px ascii
  disconfortCanvas.setCursor(0, 0);
	disconfortCanvas.setTextSize(1);
  if (discomfortIndex.GetValue() < 55)
  {
    disconfortCanvas.printf("((>_<))  \r\n");
  }
  else if(discomfortIndex.GetValue() < 75)
  {
    disconfortCanvas.printf("(^_^)    \r\n");
  }
  else
  {
    disconfortCanvas.printf("(x_x;)   \r\n");
  }

  disconfortCanvas.setTextFont(7);// 48px 7seg
  disconfortCanvas.setCursor(0, LABEL_HEIGHT);
	disconfortCanvas.setTextSize(1);
  disconfortCanvas.printf("%d\r\n", discomfortIndex.GetValue());

  // grid row2,col0
  tempCanvas.fillScreen(BLACK);

  tempCanvas.setTextFont(4);// 26px ascii
  tempCanvas.setCursor(0, 0);
	tempCanvas.setTextSize(1);
  tempCanvas.printf("%s [%s]:\r\n", temperature.GetTitle(), temperature.GetUnit());

  tempCanvas.setTextFont(7);// 48px 7seg
  tempCanvas.setCursor(0, LABEL_HEIGHT);
	tempCanvas.setTextSize(1);
  tempCanvas.printf("%2.1f\r\n", temperature.GetValue());

  // grid row2,col1
  humidCanvas.fillScreen(BLACK);
  humidCanvas.setTextFont(4);// 26px ascii
  humidCanvas.setCursor(0, 0);
	humidCanvas.setTextSize(1);
  humidCanvas.printf("%s [%s]:\r\n", humidity.GetTitle(), humidity.GetUnit());

  humidCanvas.setTextFont(7);// 48px 7seg
  humidCanvas.setCursor(0, LABEL_HEIGHT);
	humidCanvas.setTextSize(1);
  humidCanvas.printf("%2.1f\r\n", humidity.GetValue());
  
  display.startWrite(); 
  headerCanvas.pushSprite(COL0_X, ROW0_Y);
  co2Canvas.pushSprite(COL0_X, ROW1_Y);
  disconfortCanvas.pushSprite(COL1_X, ROW1_Y);
  tempCanvas.pushSprite(COL0_X, ROW2_Y);
  humidCanvas.pushSprite(COL1_X, ROW2_Y);
  display.endWrite(); 
}
