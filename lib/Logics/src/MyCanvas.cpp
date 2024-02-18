// #include "MyCanvas.h"

// // const char* ntpServer = "ntp.nict.jp";
// // const long  gmtOffset_sec = 3600 * 9;
// // const int   daylightOffset_sec = 0;

// M5Canvas canvas(&display);


// MyCanvas::MyCanvas()
// {
//     canvas.createSprite(display.width(), display.height());

// }

// void Hoge(){
//     canvas.fillScreen(BLACK);

//   // grid row0,col0
//   canvas.setTextFont(7);// 48px 7seg
//   canvas.setCursor(COL0_X, ROW0_Y);
// 	canvas.setTextSize(1);
//   canvas.printf("%02d:%02d\r\n" ,myDateTime.dt_hour ,myDateTime.dt_min);

//   // canvas.setTextFont(7);// 48px 7seg
//   // canvas.setCursor(0, 0);
// 	// canvas.setTextSize(1);
//   // canvas.printf("%02d/%02d\r\n" ,myDateTime.dt_month ,myDateTime.dt_day);

//   // grid row1,col0
//   canvas.setTextFont(4);// 26px ascii
//   canvas.setCursor(COL0_X, ROW1_Y + ROW_MARGIN);
// 	canvas.setTextSize(1);
//   canvas.printf("%s [%s]:\r\n", ccpm.GetTitle(), ccpm.GetUnit());

//   canvas.setTextFont(7);// 48px 7seg
//   canvas.setCursor(COL0_X, ROW1_Y + 26 + ROW_MARGIN);
// 	canvas.setTextSize(1);
//   canvas.printf("%4.0f\r\n", ccpm.GetValue());

//   // grid row1.col1
//   canvas.setTextFont(4);// 26px ascii
//   canvas.setCursor(COL1_X, ROW1_Y + ROW_MARGIN);
// 	canvas.setTextSize(1);
//   if (discomfortIndex.GetValue() < 55)
//   {
//     canvas.printf("((>_<))  \r\n");
//   }
//   else if(discomfortIndex.GetValue() < 75)
//   {
//     canvas.printf("(^_^)    \r\n");
//   }
//   else
//   {
//     canvas.printf("(x_x;)   \r\n");
//   }

//   canvas.setTextFont(7);// 48px 7seg
//   canvas.setCursor(COL1_X, ROW1_Y + 26 + ROW_MARGIN);
// 	canvas.setTextSize(1);
//   canvas.printf("%d\r\n", discomfortIndex.GetValue());

//   // grid row2,col0
//   canvas.setTextFont(4);// 26px ascii
//   canvas.setCursor(COL0_X, ROW2_Y + ROW_MARGIN);
// 	canvas.setTextSize(1);
//   canvas.printf("%s [%s]:\r\n", temperature.GetTitle(), temperature.GetUnit());

//   canvas.setTextFont(7);// 48px 7seg
//   canvas.setCursor(COL0_X, ROW2_Y + 26 + ROW_MARGIN);
// 	canvas.setTextSize(1);
//   canvas.printf("%2.1f\r\n", temperature.GetValue());

//   // grid row2,col1
//   canvas.setTextFont(4);// 26px ascii
//   canvas.setCursor(COL1_X, ROW2_Y + ROW_MARGIN);
// 	canvas.setTextSize(1);
//   canvas.printf("%s [%s]:\r\n", humidity.GetTitle(), humidity.GetUnit());

//   canvas.setTextFont(7);// 48px 7seg
//   canvas.setCursor(COL1_X, ROW2_Y + 26 + ROW_MARGIN);
// 	canvas.setTextSize(1);
//   canvas.printf("%2.1f\r\n", humidity.GetValue());
  
//   display.startWrite(); 
//   canvas.pushSprite(0, 0);
//   display.endWrite(); 
// }

// // void MyDateTime::Initialize()
// // {
// //   //init and get the time
// //   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  
// //   GetLocalTime();
// //   initialized = true;
// // }

// // void MyDateTime::GetLocalTime()
// // {
// //   if (initialized == false){
// //     return;
// //   }

// //   getLocalTime(&_timeinfo);
// //   dt_sec = _timeinfo.tm_sec;
// //   dt_min = _timeinfo.tm_min;
// //   dt_hour = _timeinfo.tm_hour;
// //   dt_day = _timeinfo.tm_mday;
// //   dt_month = _timeinfo.tm_mon + 1;
// //   dt_year = _timeinfo.tm_year + 1900;
// // }