#ifndef _M5ONSCREENKEYBOARD_H_
#define _M5ONSCREENKEYBOARD_H_

#include <M5Stack.h>
#include <M5ButtonDrawer.h>

class M5OnScreenKeyboard
{
public:
  bool useTextbox = true;
  bool useOver0x80Chars = false;
  bool useFACES = false;
  bool useCardKB = false;
  bool useJoyStick = false;
  bool usePLUSEncoder = false;

  uint16_t fontColor[2]   = {0xFFFF, 0xFFFF};
  uint16_t backColor[2]   = {0x630C, 0x421F};
  uint16_t frameColor[2]  = {0x0208, 0xFFFF};
  uint16_t textboxFontColor = 0x0000;
  uint16_t textboxBackColor = 0xFFFF;

  uint16_t msecHold = 300;
  uint16_t msecRepeat= 150;
  uint16_t msecMorseInput = 700;
  uint8_t maxlength = 52;

  int16_t font = 1;
  uint8_t keyHeight = 14;

  void setup(const String& value = "");
  bool loop();
  void close();

  void draw();
  void clearString();
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
  int8_t _tbl = 0;
  int8_t _col = 0;
  int8_t _row = 0;
  int8_t _cursorPos = 0;
  uint32_t _msec = 0;
  uint32_t _msecLast = 0;
  int _repeat;
  char _keyCode;
  char _pressed;
  String _string;
  uint8_t _morseInputBuf;
  bool _flgFACESKB;
  M5ButtonDrawer _btnDrawer;

  int getX(int col) const;
  int getY(int row) const;
  void updateButton();
  void switchTable();
  void inputKB(char key);
  void pressKey();
  void pressKey(char keycode);
  void clearMorse();
  void pressMorse(bool longTone);
  void inputMorse();
  void drawKeyTop(int c, int r, int x, int y, int keyh);
  void drawMorse(uint8_t m, int x, int y, uint16_t color);
  void drawTextbox();
  void drawKeyboard(int h = -1);
  void drawColumn(int col);
  void drawColumn(int col, int x, int y, int h);
  bool appear();
};

#endif
