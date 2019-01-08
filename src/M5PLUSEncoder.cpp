#include <M5PLUSEncoder.h>

M5PLUSEncoder PLUSEncoder;

bool M5PLUSEncoder::update()
{
  if (!Wire.requestFrom(0x62, 2)) return false;
  _time = millis();
  _oldUpDown = _upDown;
  _oldPress = _press;
  bool press = false;
  while (Wire.available()){
    _raw = Wire.read();
    _rawsum += _raw;
    press = (Wire.read() != 0xff);
  }
  _upDown = _rawsum / 4;
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
