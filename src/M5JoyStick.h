#ifndef _M5JOYSTICK_H_
#define _M5JOYSTICK_H_

#include <M5Stack.h>

class M5JoyStick
{
public:
  uint16_t msecHold = 300;

  void setAddr(int8_t addr) { _addr = addr; }
  void setRotate(int rotate) { _rotate = rotate; }

  bool update();

  uint8_t rawX() const { return _x; }
  uint8_t rawY() const { return _y; }
  bool isNeutral()   const { return -2 < _upDown && _upDown < 2 && -2 < _leftRight && _leftRight < 2; }
  bool isUp()        const { return -2 == _upDown; }
  bool isDown()      const { return  2 == _upDown; }
  bool isLeft()      const { return  2 == _leftRight; }
  bool isRight()     const { return -2 == _leftRight; }
  bool wasUp()       const { return -2 == _upDown && _oldUpDown != _upDown; }
  bool wasDown()     const { return  2 == _upDown && _oldUpDown != _upDown; }
  bool wasLeft()     const { return  2 == _leftRight && _oldLeftRight != _leftRight; }
  bool wasRight()    const { return -2 == _leftRight && _oldLeftRight != _leftRight; }

  bool wasClicked()  const { return _oldPress == 1 && _press == 0; }
  bool wasHold()     const { return _oldPress == 1 && _press == 2; }
  bool isHolding()   const { return _oldPress == 2 && _press == 2; }

  bool isPressed()   const { return _press; }
  bool isReleased()  const { return !_press; }
  bool wasPressed()  const { return !_oldPress && _press; }
  bool wasReleased() const { return _oldPress && !_press; }
  bool pressedFor(uint32_t ms)  const { return (_press  && _time - _lastPressedChange >= ms); }
  bool releasedFor(uint32_t ms) const { return (!_press && _time - _lastPressedChange >= ms); }
  bool directionChangedFor(uint32_t ms)  const { return (_time - _lastDirectionChange >= ms); }

private:
  int8_t _addr = 0x52;
  uint8_t _x = 0;
  uint8_t _y = 0;
  uint8_t _rotate = 0;
  int8_t _upDown = 0;
  int8_t _leftRight = 0;
  int8_t _oldUpDown = 0;
  int8_t _oldLeftRight = 0;
  uint8_t _press = 0;     // 0:release  1:click  2:holding
  uint8_t _oldPress = 0;
  uint32_t _time = 0;
  uint32_t _lastPressedChange = 0;
  uint32_t _lastDirectionChange = 0;
};

#endif

extern M5JoyStick JoyStick;

