#ifndef _M5PLUSENCODER_H_
#define _M5PLUSENCODER_H_

#include <M5Stack.h>

class M5PLUSEncoder
{
public:
  uint16_t msecHold = 300;

  bool update();

  int8_t rawValue() const { return _raw; }
  bool wasUp()      const { return _upDown > 0; }
  bool wasDown()    const { return _upDown < 0; }
  bool isPressed()  const { return _press; }
  bool wasPressed() const { return !_prevPress && _press; }
  bool wasClicked() const { return _prevPress == 1 && _press == 0; }
  bool wasHold()    const { return _prevPress == 1 && _press == 2; }
  bool isHolding()  const { return _prevPress == 2 && _press == 2; }

private:
  int8_t _raw = 0x00;
  int8_t _rawsum = 0x00;
  int8_t _upDown = 0x00;
  int8_t _prevUpDown = 0x00;
  uint8_t _press = 0x00;     // 0:release  1:click  2:holding
  uint8_t _prevPress = 0x00;
  uint32_t _pressTime = 0;
};

#endif

extern M5PLUSEncoder PLUSEncoder;

