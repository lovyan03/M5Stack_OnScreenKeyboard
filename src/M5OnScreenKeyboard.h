#ifndef _M5ONSCREENKEYBOARD_H_
#define _M5ONSCREENKEYBOARD_H_

#include <M5Stack.h>

class M5OnScreenKeyboard
{
public:
  uint16_t fontColor   = 0xFFFF;
  uint16_t backColor   = 0x630C;
  uint16_t frameColor  = 0x0208;
  uint16_t focusFontColor = 0xFFFF;
  uint16_t focusBackColor = 0x421F;
  uint16_t editFontColor = 0x0000;
  uint16_t editBackColor = 0xFFFF;
  uint8_t bottomOffset = 14;
  uint8_t keyHeight = 12;
  uint8_t textYOffset = 3;
  uint16_t msecHold = 300;
  uint16_t msecRepeat= 150;
  uint8_t maxlength = 52;

  void setup(const String& value);
  bool loop();
  void close();

  String getString() const { return _string; }
  void setString(const String& value);

private:
  enum eState
  { LEFTRIGHT
  , UPDOWN
  };
  eState _state;
  int8_t _fn = 0;
  int8_t _nowTbl = 0;
  int8_t _nowCol = 0;
  int8_t _nowRow = 0;
  int8_t _oldTbl = 0;
  int8_t _oldCol = 0;
  int8_t _oldRow = 0;
  int8_t _cursorPos = 0;
  uint32_t _msecNext = 0;
  int _repeat;
  String _string;

  int getX(int col) const;
  int getY(int row) const;
  void updateButton();
  void switchTable();
  void pressKey();
  void drawKeyTop(int c, int r, int x = -1, int y = -1);
  void drawTextbox();
  void drawKeyboard();
  void drawKeyboard(int col);
  void drawKeyboard(int col, int x, int y, int h);
};

#endif
