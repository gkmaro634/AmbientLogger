#include "Measurement.h"

Measurement::Measurement(const char* title, const char* unit)
{
  _title = title;
  _unit = unit;
}

const char* Measurement::GetTitle()
{
  return _title;
}

const char* Measurement::GetUnit()
{
  return _unit;
}

void Measurement::SetValue(float v)
{
  _value = v;
}

float Measurement::GetValue()
{
  return _value;
}