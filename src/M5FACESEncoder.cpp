#include <M5FACESEncoder.h>

M5FACESEncoder FACESEncoder;

bool M5FACESEncoder::update()
{
  if (!Wire.requestFrom(_addr, 2)) return false;
  _time = millis();
  _oldUpDown = _upDown;
  _oldPress = _press;
  bool press = false;
  while (Wire.available()){
    _raw = Wire.read();
    _rawsum += _raw;
    press = (Wire.read() == 0);
  }
  _upDown = _rawsum;
  if (_upDown != 0) _rawsum = 0;

  if (press != (0 != _oldPress)) _lastChange = _time;
  if (press) {
    if (!_oldPress) {
      _press = 1;
    } else 
    if (1 == _oldPress && (_time - _lastChange >= msecHold)) {
      _press = 2;
    }
  } else {
    _press = 0;
  }

  return true;
}

void M5FACESEncoder::led(int led_index, int r, int g, int b)
{
  Wire.beginTransmission(_addr);
  Wire.write(led_index);
  Wire.write(r);
  Wire.write(g);
  Wire.write(b);
  Wire.endTransmission();
}
