#include <M5PLUSEncoder.h>

M5PLUSEncoder PLUSEncoder;

bool M5PLUSEncoder::update()
{
  if (!Wire.requestFrom(0x62, 2)) return false;
  _prevUpDown = _upDown;
  _prevPress = _press;
  bool press = false;
  while (Wire.available()){
    _raw = Wire.read();
    _rawsum += _raw;
    press = (Wire.read() != 0xff);
  }
  _upDown = _rawsum / 4;
  if (_upDown != 0) _rawsum = 0;

  if (press) {
    uint32_t msec = millis();
    if (!_prevPress) {
      _press = 1;
      _pressTime = msec;
    } else 
    if (1 == _prevPress && (_pressTime + msecHold < msec)) {
      _press = 2;
    }
  } else {
    _press = 0;
  }
  return true;
}
