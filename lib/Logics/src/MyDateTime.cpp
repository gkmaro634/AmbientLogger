#include "MyDateTime.h"

const char* ntpServer = "ntp.nict.jp";
const long  gmtOffset_sec = 3600 * 9;
const int   daylightOffset_sec = 0;

MyDateTime::MyDateTime()
{
}

void MyDateTime::Initialize()
{
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  
  GetLocalTime();
  initialized = true;
}

void MyDateTime::GetLocalTime()
{
  if (initialized == false){
    return;
  }

  getLocalTime(&_timeinfo);
  dt_sec = _timeinfo.tm_sec;
  dt_min = _timeinfo.tm_min;
  dt_hour = _timeinfo.tm_hour;
  dt_day = _timeinfo.tm_mday;
  dt_month = _timeinfo.tm_mon + 1;
  dt_year = _timeinfo.tm_year + 1900;
}