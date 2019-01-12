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
  uint16_t textboxFontColor = 0x0000;
  uint16_t textboxBackColor = 0xFFFF;
  uint8_t bottomOffset = 14;
  uint8_t keyHeight = 14;
  uint8_t textYOffset = 4;
  uint16_t msecHold = 300;
  uint16_t msecRepeat= 150;
  uint8_t maxlength = 52;

  bool useTextbox = true;
  bool useFACES = false;
  bool useJoyStick = false;
  bool usePLUSEncoder = false;

  void setup(const String& value = "");
  bool loop();
  void close();

  void draw();

  String getString() const { return _string; }
  void setString(const String& value = "");
  char getKeyCode() const { return _keyCode; }
private:
  enum eState
  { APPEAR
  , LEFTRIGHT
  , UPDOWN
  , MORSE
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
  uint32_t _msec = 0;
  uint32_t _msecNext = 0;
  int _repeat;
  char _keyCode;
  String _string;
  int8_t _morsePos;
  int16_t _morseInputBuf;

  int getX(int col) const;
  int getY(int row) const;
  void updateButton();
  void switchTable();
  void pressKey();
  void pressKey(char keycode);
  void clearMorse();
  void pressMorse(bool longTone);
  void inputMorse();
  void drawKeyTop(int c, int r, int x, int y);
  void drawMorse(uint16_t m, int x, int y);
  void drawTextbox();
  void drawKeyboard(int h = -1);
  void drawColumn(int col);
  void drawColumn(int col, int x, int y, int h);
  bool appear();
};

#endif
