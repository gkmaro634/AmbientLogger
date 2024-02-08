#include "MHZ19C.h"

/* Motor()
*/
MHZ19C::MHZ19C(int pwmPin)
{
  _pwmPin=pwmPin;
	pinMode(_pwmPin, INPUT);
  // Wire.begin();
}



byte MHZ19C::get()
{
  while(true)
  {
    if (digitalRead(_pwmPin) != LOW)
    {
      break;
    }
  }

	_outputHighDuration = pulseIn(_pwmPin, HIGH, 3000 * 1000);// TODO: other thread
	if(_outputHighDuration == 0)
  {
    ccpm = 0.0;
    return 1;
  }

		float th = (float)_outputHighDuration / 1000.0;
		ccpm = 2.0 * (th - 2.0);

    return 0;
}