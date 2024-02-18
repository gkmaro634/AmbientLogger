#include "Measurement.h"

Measurement::Measurement(const char* title, const char* unit)
{
  _title = title;
  _unit = unit;
    
  _xQueue = xQueueCreate(1, sizeof(float));
  if(_xQueue == NULL)
  {
    _title = "invlalid";
  }  
}

const char* Measurement::GetTitle()
{
  return _title;
}

const char* Measurement::GetUnit()
{
  return _unit;
}

int Measurement::Enqueue(float sendValue){
  BaseType_t result;

  result = xQueueOverwrite(_xQueue, &sendValue);
  if (result != pdPASS)
  {
    return 1;
  }
  return 0;
}

int Measurement::Dequeue(float *receiveValue, int timeoutMs){
    BaseType_t result;

    result = xQueueReceive(_xQueue, receiveValue, pdMS_TO_TICKS(timeoutMs));
    if ( result != pdPASS)
    {
      return 1;
    }

    return 0;
}

int Measurement::Peek(float *receiveValue, int timeoutMs){
    BaseType_t result;

    result = xQueuePeek(_xQueue, receiveValue, pdMS_TO_TICKS(timeoutMs));
    if ( result != pdPASS)
    {
      return 1;
    }

    return 0;
}