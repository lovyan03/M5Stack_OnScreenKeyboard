#ifndef _M5ONSCREENKEYBOARD_H_
#define _M5ONSCREENKEYBOARD_H_

#include <M5Stack.h>
#include "M5ButtonDrawer.h"

class M5OnScreenKeyboard
{
public:
  uint16_t editFontColor = 0x0000;
  uint16_t editFillColor = 0xFFFF;
  uint16_t fontColor   = 0xFFFF;
  uint16_t fillColor   = 0x630C;
  uint16_t frameColor  = 0x0208;
  uint16_t cursorColor = 0x421F;
  uint8_t bottomOffset = 14;
  uint8_t keyHeight = 12;
  uint8_t textYOffset = 3;
  uint16_t msecHold = 300;
  uint16_t msecRepeat= 150;
  uint8_t maxlength = 52;

  String getString() { return _string; }
  void setString(const String& value) {
    _string = value; drawTextbox();
    _cursorPos = _string.length();
  }

  void setup(const String& value) {
    _string = value; drawTextbox();
    _cursorPos = _string.length();
  
    drawKeyboard();
    drawTextbox();
    _nowTbl = 0;
    _nowCol = 0;
    _nowRow = 0;
    _state = LEFTRIGHT;
    _msecNext = 0;
  }
  bool loop() {
    uint32_t msec = millis();

    // draw blink cursor.
    M5.Lcd.drawFastVLine(_cursorPos * 6, getY(-1) + textYOffset, 8, (msec / 150) % 2 ? 0xffff : 0);

    if (msec < _msecNext) return true;

    bool mod = false;
    M5.update();

    // BtnA Pressing Fn _state
    if (M5.BtnA.isPressed()) {
      if (M5.BtnB.wasPressed()) {
        mod = true;
        switchTable();
      }
      if (M5.BtnC.wasPressed()) {
        if (_state == LEFTRIGHT) return false;
        mod = true; 
        _state = LEFTRIGHT;
      }
    } else {
      switch (_state) {
      case LEFTRIGHT:   // left right moving
        if (M5.BtnA.wasReleased() && !M5.BtnA.wasReleasefor(msecHold)) { --_nowCol; }
        if (M5.BtnB.wasPressed() || M5.BtnB.pressedFor(msecHold)) { if (++_repeat < COLUMNCOUNT) ++_nowCol; }
        if (M5.BtnC.wasPressed()) { mod = true; _state = UPDOWN; }
        break;
      case UPDOWN:    // up down moving
        if (M5.BtnA.wasReleased() && !M5.BtnA.wasReleasefor(msecHold)) { --_nowRow; }
        if (M5.BtnB.wasPressed() || M5.BtnB.pressedFor(msecHold)) { if (++_repeat < ROWCOUNT) ++_nowRow; }
        if (M5.BtnC.wasPressed() || M5.BtnC.pressedFor(msecHold)) { mod = true; ++_repeat; if (!pressKey()) return false; }
        if (M5.BtnC.wasReleased() && _repeat) { mod = true; _state = LEFTRIGHT; }

        break;
      }
    }
#ifdef _PLUSEncoder_H_
    if (PlusEncoder.update()) {
      switch (_state) {
      case LEFTRIGHT:   // left right moving
        if (PlusEncoder.isLongClick()) { mod = true; switchTable(); break; } 
        if (PlusEncoder.wasUp())       { --_nowCol; }
        if (PlusEncoder.wasDown())     { ++_nowCol; }
        if (PlusEncoder.isClick())     { mod = true; _state = UPDOWN; }
        break;
      case UPDOWN:    // up down moving
        if (PlusEncoder.isLongClick()) { mod = true; _state = LEFTRIGHT; }
        if (PlusEncoder.wasUp())       { --_nowRow; }
        if (PlusEncoder.wasDown())     { ++_nowRow; }
        if (PlusEncoder.isClick())     { mod = true; if (!pressKey()) return false; }
        break;
      }
    }
#endif
    updateButton();
    buttonDrawer.draw();
    if (_oldCol != _nowCol || _oldRow != _nowRow || _oldTbl != _nowTbl || mod) {
      _nowCol = (_nowCol + COLUMNCOUNT) % COLUMNCOUNT;
      _nowRow = (_nowRow + ROWCOUNT   ) % ROWCOUNT;
      if (_oldTbl != _nowTbl) {
        _oldTbl = _nowTbl;
        drawKeyboard();
      } else {
        drawKeyboard(_nowCol);
        if (_oldCol != _nowCol) drawKeyboard(_oldCol);
      }
      if (0 < _repeat) _msecNext = msec + msecRepeat;
      _oldCol = _nowCol;
      _oldRow = _nowRow;
    } else {
      if (M5.BtnB.isReleased() && M5.BtnC.isReleased()) _repeat = 0;
      _msecNext = 0;
    }
    return true;
  }

  void close() { 
    int y = getY(-1);
    M5.Lcd.fillRect(0, y, TFT_HEIGHT, TFT_WIDTH - y, 0);
    _string = "";
  }

  bool isOk() { return _isOk; }

private:
  enum 
  { TABLECOUNT  = 3
  , ROWCOUNT    = 4
  , COLUMNCOUNT =11
  , KEYWIDTH   = 29
  };

  static const char BS   = 0x08;
  static const char DEL  = 0x7f;
  static const char LEFT = 0x1d;
  static const char RIGH = 0x1c;

  const PROGMEM char _chartbl[TABLECOUNT][ROWCOUNT][COLUMNCOUNT] 
     = {{{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', BS }
       , {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', DEL}
       , {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '/',LEFT}
       , {'z', 'x', 'c', 'v', 'b', 'n', 'm', ' ', '.', '@',RIGH}
       }
      , {{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', BS }
       , {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', DEL}
       , {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '/',LEFT}
       , {'Z', 'X', 'C', 'V', 'B', 'N', 'M', ' ', '.', '@',RIGH}
       }
      , {{'!', '"', '#', '$', '%', '&','\'', '`', '^', '~', BS }
       , {'<', '>', '[', ']', '{', '}', '(', ')', '=','\\', DEL}
       , {'_', '|', ',', ';', ':', '?', '+', '*', '-', '/',LEFT}
       , {'.', '.', '.', '.', '.', '.', '.', ' ', '.', '@',RIGH}
      }};
  enum eState
  { LEFTRIGHT
  , UPDOWN
  };
  eState _state;
  int8_t _nowTbl = 0;
  int8_t _nowCol = 0;
  int8_t _nowRow = 0;
  int8_t _oldTbl = 0;
  int8_t _oldCol = 0;
  int8_t _oldRow = 0;
  int8_t _cursorPos = 0;
  uint32_t _msecNext = 0;
  int _repeat;
  bool _isOk = false;
  String _string;
  M5ButtonDrawer buttonDrawer;

  int getX(int col) { return col * KEYWIDTH; }
  int getY(int row) { return TFT_WIDTH - bottomOffset - (ROWCOUNT - row) * keyHeight; }

  void updateButton() {
    if (M5.BtnA.isPressed()) {
      if (M5.BtnA.pressedFor(msecHold)) {
        buttonDrawer.setText(0, "Fn");
      } else {
        switch (_state) {
        case LEFTRIGHT: buttonDrawer.setText("Left", "Panel", "Finish"); break;
        case UPDOWN:    buttonDrawer.setText("Up", "Panel", "Column"); break;
        }
      }
    } else {
      switch (_state) {
      case LEFTRIGHT: buttonDrawer.setText("Left/Fn", "Right", "Row"); break;
      case UPDOWN:    buttonDrawer.setText("Up/Fn", "Down", "Ok"); break;
      }
    }
  }
  void switchTable() {
    _nowTbl = ++_nowTbl % TABLECOUNT;
  }

  bool pressKey() {
    switch (_chartbl[_nowTbl][_nowRow][_nowCol]) {
    case BS  :  if (0 < _cursorPos) {
                  _string = _string.substring(0, _cursorPos-1) + _string.substring(_cursorPos);
                  --_cursorPos;
                }
                break;
    case DEL :  if (_cursorPos < _string.length()) {
                  _string = _string.substring(0, _cursorPos) + _string.substring(_cursorPos+1);
                }
                break;
    case LEFT:  _cursorPos -= (0 < _cursorPos ? 1 : 0); break;
    case RIGH:  _cursorPos += (_string.length() > _cursorPos ? 1 : 0); break;
    default:
      if (_string.length() < maxlength) {
        _string = _string.substring(0, _cursorPos) + _chartbl[_nowTbl][_nowRow][_nowCol] + _string.substring(_cursorPos);
        ++_cursorPos;
      }
      break;
    }
    drawTextbox();
    return true;
  }

  void drawKeyTop(int c, int r, int x = -1, int y = -1) {
    if (x == -1) x = getX(c);
    if (y == -1) y = getY(r);
    M5.Lcd.setCursor(x, y + textYOffset);
    switch (_chartbl[_nowTbl][r][c]) {
    case BS  :  M5.Lcd.print("<B S ");  break;
    case DEL :  M5.Lcd.print(" DEL>");  break;
    case LEFT:  M5.Lcd.print(" <<  ");  break;
    case RIGH:  M5.Lcd.print("  >> ");  break;
    default:
      M5.Lcd.setCursor(x + 13, y + textYOffset);
      M5.Lcd.print(_chartbl[_nowTbl][r][c]);
      break;
    }
  }

  void drawTextbox() {
    M5.Lcd.setTextColor(0x0000);
    int y = getY(-1);
    M5.Lcd.setCursor(1, y + textYOffset);
    M5.Lcd.drawFastHLine(0, y, TFT_HEIGHT, frameColor);
    M5.Lcd.fillRect(0, y + 1, TFT_HEIGHT, keyHeight - 1, 0xffff);
    M5.Lcd.print(_string);
  }

  void drawKeyboard() {
    int y = getY(0);
    int h = getY(ROWCOUNT) - y;
    for (int c = 0; c < COLUMNCOUNT; c++) {
      int x = getX(c);
      drawKeyboard(c, x, y, h);
      M5.Lcd.drawFastVLine(x, y, h, frameColor);
    }
    M5.Lcd.drawFastVLine(getX(COLUMNCOUNT), y, h, frameColor);
  }

  void drawKeyboard(int col) {
    drawKeyboard(col, getX(col), getY(0), ROWCOUNT * keyHeight);
  }

  void drawKeyboard(int col, int x, int y, int h) {
    M5.Lcd.setTextColor(fontColor);
    M5.Lcd.fillRect(x+1, y, KEYWIDTH-1, h, fillColor);
    if (_nowCol == col) {
      M5.Lcd.fillRect(x+1, getY(_nowRow)+1, KEYWIDTH - 1, keyHeight - 1, cursorColor);
      if (_state == LEFTRIGHT) {
        M5.Lcd.drawRect(x+1, y+1, KEYWIDTH - 1, h - 1, cursorColor);
        M5.Lcd.drawRect(x+2, y+2, KEYWIDTH - 3, h - 3, cursorColor);
      }
    }
    for (int r = 0; r < ROWCOUNT; r++) {
      y = getY(r);
      drawKeyTop(col, r, x, y);
      M5.Lcd.drawFastHLine(x, y, KEYWIDTH, frameColor);
    }
  }
};

#endif
