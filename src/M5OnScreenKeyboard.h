#ifndef _M5ONSCREENKEYBOARD_H_
#define _M5ONSCREENKEYBOARD_H_

#include <M5Stack.h>
#include <M5ButtonDrawer.h>

class M5OnScreenKeyboard
{
public:
  static bool useTextbox;
  static bool useOver0x80Chars;
  static bool useFACES;
  static bool useCardKB;
  static bool useJoyStick;
  static bool usePLUSEncoder;
  static bool swapBtnBC;

  static uint16_t fontColor[2];
  static uint16_t backColor[2];
  static uint16_t frameColor[2];
  static uint16_t textboxFontColor;
  static uint16_t textboxBackColor;
  static uint8_t keyHeight;

  static uint16_t msecHold;
  static uint16_t msecRepeat;
  static uint16_t msecMorseInput;
  static uint8_t maxlength;

  static void setTextFont(int f) { gfxFont = NULL; font = f; }
  static void setFreeFont(const GFXfont* f) { gfxFont = f; font = 1; }

  void setup(const String& value = "");
  bool loop();
  void close();

  void draw();
  void clearString();
  String getString() const { return _string; }
  void setString(const String& value = "");
  char getKeyCode() const { return _keyCode; }
private:
  static int16_t font;
  static const GFXfont* gfxFont;

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
  bool inputKB(char key);
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
  void applyFont();
  bool appear();
};

#endif
