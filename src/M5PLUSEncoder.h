#ifndef _M5PLUSENCODER_H_
#define _M5PLUSENCODER_H_

#include <M5Stack.h>

class M5PLUSEncoder
{
public:
  uint16_t msecHold = 300;

  void setAddr(int8_t addr) { _addr = addr; }

  bool update();

  int8_t rawValue() const { return _raw; }
  bool wasUp()      const { return _upDown > 0; }
  bool wasDown()    const { return _upDown < 0; }

  bool wasClicked()  const { return _oldPress == 1 && _press == 0; }
  bool wasHold()     const { return _oldPress == 1 && _press == 2; }
  bool isHolding()   const { return _oldPress == 2 && _press == 2; }

  bool isPressed()   const { return _press; }
  bool isReleased()  const { return !_press; }
  bool wasPressed()  const { return !_oldPress && _press; }
  bool wasReleased() const { return _oldPress && !_press; }
  bool pressedFor(uint32_t ms)  const { return (_press  && _time - _lastChange >= ms); }
  bool releasedFor(uint32_t ms) const { return (!_press && _time - _lastChange >= ms); }

private:
  int8_t _addr = 0x62;
  int8_t _raw = 0;
  int8_t _rawsum = 0;
  int8_t _upDown = 0;
  int8_t _oldUpDown = 0;
  uint8_t _press = 0;     // 0:release  1:click  2:holding
  uint8_t _oldPress = 0;
  uint32_t _time = 0;
  uint32_t _lastChange = 0;
};

#endif

extern M5PLUSEncoder PLUSEncoder;

