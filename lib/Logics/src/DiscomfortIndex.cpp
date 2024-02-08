#include "DiscomfortIndex.h"

DiscomfortIndex::DiscomfortIndex()
{
  _value = 0;
}

void DiscomfortIndex::Update(float temp, float hum)
{
  _value = 0.81 * temp + hum * 0.01 * (0.99 * temp - 14.3) + 46.3;
}

int DiscomfortIndex::GetValue()
{
  return _value;
}
