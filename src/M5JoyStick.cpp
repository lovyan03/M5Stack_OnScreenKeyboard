#include <M5JoyStick.h>

M5JoyStick JoyStick;

static int ValueConv(int v) { return (v < 48) ? -2 : (v < 64) ? -1 : (v > 207) ?  2 : (v > 191) ?  1 : 0; }

bool M5JoyStick::update()
{
  if (!Wire.requestFrom(_addr,3)) return false;
  _time = millis();
  _oldPress = _press;
  _oldUpDown = _upDown;
  _oldLeftRight = _leftRight;
  bool press = false;
  while (Wire.available()) {
    _x = Wire.read();
    _y = Wire.read();
    press = Wire.read();
    switch (_rotate) {
    default:
      _upDown    =  ValueConv(_y);
      _leftRight =  ValueConv(_x);
      break;
    case 1:
      _upDown    = -ValueConv(_x);
      _leftRight =  ValueConv(_y);
      break;
    case 2:
      _upDown    = -ValueConv(_y);
      _leftRight = -ValueConv(_x);
      break;
    case 3:
      _upDown    =  ValueConv(_x);
      _leftRight = -ValueConv(_y);
      break;
    }
    if (_oldUpDown    == (_upDown    * 2))  _upDown    = _oldUpDown;
    if (_oldLeftRight == (_leftRight * 2))  _leftRight = _oldLeftRight;
  }
  if (press != _oldPress) _lastChange = _time;
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
