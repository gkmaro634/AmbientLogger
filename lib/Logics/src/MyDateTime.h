#ifndef __MYDATETIME_H
#define __MYDATETIME_H

#include <Arduino.h>
#include <M5Core2.h>
#include "time.h"

class MyDateTime{
public:
  MyDateTime();
  void Initialize();
  void GetLocalTime();
  int dt_sec=0;
  int dt_min=0;
  int dt_hour=0;
  int dt_day=0;
  int dt_month=0;
  int dt_year=0;

private:
  struct tm _timeinfo;
};


#endif