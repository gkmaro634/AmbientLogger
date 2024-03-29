#include "main.h"

// valiables
WiFiClient client;

MyDateTime myDateTime;
M5GFX display;
M5Waveform waveform(&display);

M5Canvas headerCanvas(&display);
M5Canvas co2Canvas(&display);
M5Canvas disconfortCanvas(&display);
M5Canvas tempCanvas(&display);
M5Canvas humidCanvas(&display);
M5Canvas waveChartCanvas(&display);

Measurement temperature("Temp.", "C");
Measurement humidity("Hum.", "%");
Measurement ccpm("CO2", "ppm");
DiscomfortIndex discomfortIndex;

SHT3X sht30;
MHZ19C mhz19c;

bool hasPressed;
ModeType mode = INDICATOR;

TaskHandle_t handleWifiConnectTask;
TaskHandle_t handlePrintTask;

TimerHandle_t handleCheckWifiStateTimer;
TimerHandle_t handleAcqTimer;

void setup()
{
  // put your setup code here, to run once:
  BaseType_t status;

  // デバイス初期化
  M5.begin();
  Serial.begin(UART_BAUDRATE);
  Wire.begin();
  mhz19c = MHZ19C(MHZ19C_PWM_PIN);
  Serial.println("Device initialized.");

  // 画面初期化
  display.init();
  display.setBrightness(64);
  headerCanvas.createSprite(HEADER_WIDTH, HEADER_HEIGHT);
 
  // Indicator Mode
  co2Canvas.createSprite(BLOCK_WIDTH, BLOCK_HEIGHT);
  disconfortCanvas.createSprite(BLOCK_WIDTH, BLOCK_HEIGHT);
  tempCanvas.createSprite(BLOCK_WIDTH, BLOCK_HEIGHT);
  humidCanvas.createSprite(BLOCK_WIDTH, BLOCK_HEIGHT);

  // WaveChart Mode
  waveChartCanvas.createSprite(CHART_SPRITE_WIDTH, CHART_SPRITE_HEIGHT);
  waveform.init(CHART_WIDTH, CHART_HEIGHT, 9, 9);
  waveform.updateXAxisDiv(30);// 30s*10=300s
  waveform.updateYAxisDiv(100);// 100*10=1000s
  Serial.println("Display initialized.");

  // Task初期化
  disableCore0WDT();
  Serial.println("disableCore0WDT.");
  disableCore1WDT();
  Serial.println("disableCore1WDT.");

  status = xTaskCreatePinnedToCore(connectWifiTask, "wifiTask", 4096, NULL, 1, &handleWifiConnectTask, 1);
  configASSERT(status == pdPASS);
  Serial.println("wifiTask Created.");

  handleCheckWifiStateTimer = xTimerCreate("checkWifiStateTask", pdMS_TO_TICKS(500), pdTRUE, NULL, checkWifiStateTask);
  handleAcqTimer = xTimerCreate("acqTask", pdMS_TO_TICKS(ACQ_INTERVAL_MS), pdTRUE, NULL, acquisitionTask);
  Serial.println("timers Created.");
}

void loop()
{
  delay(50);
  M5.update();
  auto detail = M5.Touch.getDetail();

  if (hasPressed == true)
  {
    if (detail.isReleased())
    {
      hasPressed = false;
      Serial.println("Touch detected.");
      if (mode == INDICATOR)
      {
        mode = WAVE_CHART;
      }
      else
      {
        mode = INDICATOR;
      }
    }
  }
  else
  {
    hasPressed = detail.isPressed();
  }
  // vTaskDelete(NULL);
}

void connectWifiTask(void *arg)
{
  // 接続開始
  WiFi.begin(ssid, pass);
  display.println("Start WiFi connection");
  Serial.println("Start WiFi connection.");

  // 状態監視開始
  xTimerStart(handleCheckWifiStateTimer, 0);
  Serial.println("xTimerStart(handleCheckWifiStateTimer, 0);");

  // 通知を待つ
  if (xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(WIFI_CONNECT_TIMEOUT_MS)))
  {
    // 接続完了の場合
    myDateTime.Initialize();
    display.println("WiFi connected");
    Serial.println("WiFi connected");
  }
  else
  {
    // タイムアウトの場合
    display.println("WiFi connection timed out");
    Serial.println("WiFi connection timed out");
  }

  xTimerStop(handleCheckWifiStateTimer, 0);
  Serial.println("xTimerStop(handleCheckWifiStateTimer, 0)");

  // 描画更新開始
  waveform.startDrawing(64, updateWaveChart);

  // メイン処理初回実行
  display.fillScreen(BLACK);
  acquisitionTask(NULL);
  Serial.println("acquisitionTask and printTask launched");

  // メイン処理タイマ開始
  xTimerStart(handleAcqTimer, 0);
  Serial.println("xTimerStart(handleAcqTimer, 0)");

  auto status = xTaskCreatePinnedToCore(printTask, "printTask", 8192, NULL, 1, &handlePrintTask, 1);
  configASSERT(status == pdPASS);
  Serial.println("printTask Created.");

  vTaskDelete(NULL);
  Serial.println("vTaskDelete(NULL)");
}

void checkWifiStateTask(void *arg)
{
  // print "."
  display.print(".");

  // 状態を取得し接続完了なら通知する
  if (WiFi.status() == WL_CONNECTED)
  {
    // 通知
    xTaskNotifyGive(handleWifiConnectTask);
  }
}

void acquisitionTask(void *arg)
{
  if (mhz19c.get() == 0)
  {
    ccpm.Enqueue(mhz19c.ccpm);
    waveform.enqueue(mhz19c.ccpm);
  }

  if (sht30.get() == 0)
  {
    temperature.Enqueue(sht30.cTemp);
    humidity.Enqueue(sht30.humidity);
    discomfortIndex.Update(sht30.cTemp, sht30.humidity);
  }
}

void updateIndicator()
{
  // grid row1,col0
  co2Canvas.fillScreen(BLACK);
  co2Canvas.setTextFont(4); // 26px ascii
  co2Canvas.setCursor(0, 0);
  co2Canvas.setTextSize(1);
  co2Canvas.printf("%s [%s]:\r\n", ccpm.GetTitle(), ccpm.GetUnit());

  co2Canvas.setTextFont(7); // 48px 7seg
  co2Canvas.setCursor(0, LABEL_HEIGHT);
  co2Canvas.setTextSize(1);

  float ccpmValue; // = 1.0;
  if (ccpm.Peek(&ccpmValue, 100) == 0)
  {
    co2Canvas.printf("%4.0f\r\n", ccpmValue);
  }

  // grid row1.col1
  disconfortCanvas.fillScreen(BLACK);
  disconfortCanvas.setTextFont(4); // 26px ascii
  disconfortCanvas.setCursor(0, 0);
  disconfortCanvas.setTextSize(1);
  if (discomfortIndex.GetValue() < 55)
  {
    disconfortCanvas.printf("((>_<))  \r\n");
  }
  else if (discomfortIndex.GetValue() < 75)
  {
    disconfortCanvas.printf("(^_^)    \r\n");
  }
  else
  {
    disconfortCanvas.printf("(x_x;)   \r\n");
  }

  disconfortCanvas.setTextFont(7); // 48px 7seg
  disconfortCanvas.setCursor(0, LABEL_HEIGHT);
  disconfortCanvas.setTextSize(1);
  disconfortCanvas.printf("%d\r\n", discomfortIndex.GetValue());

  // grid row2,col0
  tempCanvas.fillScreen(BLACK);

  tempCanvas.setTextFont(4); // 26px ascii
  tempCanvas.setCursor(0, 0);
  tempCanvas.setTextSize(1);
  tempCanvas.printf("%s [%s]:\r\n", temperature.GetTitle(), temperature.GetUnit());

  tempCanvas.setTextFont(7); // 48px 7seg
  tempCanvas.setCursor(0, LABEL_HEIGHT);
  tempCanvas.setTextSize(1);
  float tempValue;
  if (temperature.Peek(&tempValue, 100) == 0)
  {
    tempCanvas.printf("%2.1f\r\n", tempValue);
  }

  // grid row2,col1
  humidCanvas.fillScreen(BLACK);
  humidCanvas.setTextFont(4); // 26px ascii
  humidCanvas.setCursor(0, 0);
  humidCanvas.setTextSize(1);
  humidCanvas.printf("%s [%s]:\r\n", humidity.GetTitle(), humidity.GetUnit());

  humidCanvas.setTextFont(7); // 48px 7seg
  humidCanvas.setCursor(0, LABEL_HEIGHT);
  humidCanvas.setTextSize(1);
  float humidValue;
  if (humidity.Peek(&humidValue, 100) == 0)
  {
    humidCanvas.printf("%2.1f\r\n", humidValue);
  }
}

void updateWaveChart(void)
{
  // grid row1,col1
  waveChartCanvas.fillScreen(BLACK);
  waveChartCanvas.setTextFont(4); // 26px ascii
  waveChartCanvas.setCursor(CHART_TITLE_X, CHART_TITLE_Y);
  waveChartCanvas.setTextSize(1);
  waveChartCanvas.printf("%s [%s]:\r\n", ccpm.GetTitle(), ccpm.GetUnit());

  waveform.figureCanvas->pushSprite(&waveChartCanvas, CHART_X, CHART_Y, BLACK);
}

void printTask(void *arg)
{
  while (true)
  {
    delay(PRINT_INTERVAL_MS);

    myDateTime.GetLocalTime();

    // grid row0,col0
    headerCanvas.fillScreen(BLACK);
    headerCanvas.setTextFont(7); // 48px 7seg
    headerCanvas.setCursor(0, 0);
    headerCanvas.setTextSize(1);
    headerCanvas.printf("%02d:%02d\r\n", myDateTime.dt_hour, myDateTime.dt_min);

    // canvas.setTextFont(7);// 48px 7seg
    // canvas.setCursor(0, 0);
    // canvas.setTextSize(1);
    // canvas.printf("%02d/%02d\r\n" ,myDateTime.dt_month ,myDateTime.dt_day);

    updateIndicator();

    // updateWaveChart();

    display.startWrite();
    headerCanvas.pushSprite(COL0_X, ROW0_Y);
    if (mode == INDICATOR)
    {
      co2Canvas.pushSprite(COL0_X, ROW1_Y);
      disconfortCanvas.pushSprite(COL1_X, ROW1_Y);
      tempCanvas.pushSprite(COL0_X, ROW2_Y);
      humidCanvas.pushSprite(COL1_X, ROW2_Y);
    }
    else if (mode == WAVE_CHART)
    {
      waveChartCanvas.pushSprite(CHART_SPRITE_X, CHART_SPRITE_Y);
    }
    else
    {
    }
    display.endWrite();
  }
}
