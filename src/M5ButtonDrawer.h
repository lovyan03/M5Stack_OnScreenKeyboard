#ifndef _M5BUTTONDRAWER_H_
#define _M5BUTTONDRAWER_H_

#include <M5Stack.h>

class M5ButtonDrawer {
public:
  uint16_t frameColor[2] = { 0xA514,0xffff };
  uint16_t backColor[2]  = { 0x0000,0x0000 };
  uint16_t fontColor[2]  = { 0xffff,0xffff };
  uint16_t width = 64;
  uint16_t height = 14;
  M5ButtonDrawer(){};
  M5ButtonDrawer(const String& btnA, const String& btnB, const String& btnC)
   : _titles{btnA,btnB,btnC}
  {
  };
  void setText(const String& btnA, const String& btnB, const String& btnC);
  void setText(uint8_t idx, const String& str);
  void draw() const;
  void draw(uint8_t index, bool pressed) const;

private:
  String _titles[3];
  void draw(uint16_t x, bool pressed, const String& title) const;
};

#endif

extern M5ButtonDrawer ButtonDrawer;

