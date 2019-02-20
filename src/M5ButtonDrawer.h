#ifndef _M5BUTTONDRAWER_H_
#define _M5BUTTONDRAWER_H_

#include <M5Stack.h>

class M5ButtonDrawer {
public:
  static uint16_t frameColor[2];
  static uint16_t backColor[2];
  static uint16_t fontColor[2];
  static int16_t width;
  static int16_t height;
  static void setTextFont(int f) { gfxFont = NULL; font = f; }
  static void setFreeFont(const GFXfont* f) { gfxFont = f; font = 1; }
  M5ButtonDrawer(){};
  M5ButtonDrawer(const String& btnA, const String& btnB, const String& btnC)
   : _titles{btnA,btnB,btnC}
  {
  };
  void setText(const String& btnA, const String& btnB, const String& btnC);
  void setText(uint8_t idx, const String& str);
  void draw(bool force = false);
  void draw(uint8_t idx, bool pressed);

private:
  static int16_t font;
  static const GFXfont* gfxFont;
  String _titles[3];
  bool _mod[3];
  void drawButton(int x, bool pressed, const String& title) const;
};

#endif

