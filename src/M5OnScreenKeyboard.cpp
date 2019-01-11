#include <M5OnScreenKeyboard.h>
#include <M5Stack.h>

#include <M5ButtonDrawer.h>
#include <M5PLUSEncoder.h>
#include <M5JoyStick.h>

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

static const PROGMEM char _chartbl[TABLECOUNT][ROWCOUNT][COLUMNCOUNT] 
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


// morse code 1tone:2bit encode ( Start with LSB )
// 00 none
// 01 short
// 11 long
// 10 nothing
// е   1 or 4
// -   3 or c
// ее  5
// -е  7
// е-  d
// --  f
// ( SPACE & BACKSPACE morse code conforms to Google input. )
static const PROGMEM uint16_t _morsetbl[2][ROWCOUNT][COLUMNCOUNT]
   = {{{0x03fd, 0x03f5, 0x03d5, 0x0355, 0x0155, 0x0157, 0x015f, 0x017f, 0x01ff, 0x03ff, 0x00ff}
     , {0x00df, 0x003d, 0x0001, 0x001d, 0x0003, 0x00f7, 0x0035, 0x0005, 0x003f, 0x007d,  DEL}
     , {0x000d, 0x0015, 0x0017, 0x0075, 0x001f, 0x0055, 0x00fd, 0x0037, 0x005d, 0x01d7, LEFT}
     , {0x005f, 0x00d7, 0x0077, 0x00d5, 0x0057, 0x0007, 0x000f, 0x00f5, 0x0ddd, 0x077d, RIGH}
     }
    , {{0x0f77, 0x075d, 0x01df, 0x05d5, 0x037f, 0x015d, 0x07fd, 0x0dd7, 0x0555, 0x05fd, 0x00ff}
     , {0x03f7, 0x0ff7, 0x017d, 0x0d7d, 0x037d, 0x0f7d, 0x01f7, 0x0df7, 0x0357, 0x0377,  DEL}
     , {0x0df5, 0x0ddf, 0x0f5f, 0x0777, 0x057f, 0x05f5, 0x01dd, 0x01d5, 0x0d57, 0x01d7, LEFT}
     , {     0,      0,      0,      0,      0,      0,      0, 0x00f5, 0x0ddd, 0x077d, RIGH}
    }};

static int numofbits(uint16_t bits)
{
    bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
    bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
    bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
    return (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
}

void M5OnScreenKeyboard::setString(const String& value) {
  _string = value;
  _cursorPos = _string.length();
}

void M5OnScreenKeyboard::setup(const String& value) {
  ButtonDrawer.setText("","","");
  ButtonDrawer.draw();
  setString(value);
  _nowTbl = 0;
  _nowCol = 0;
  _nowRow = 0;
  _state = APPEAR;
  _msecNext = 0;
  _msec = millis();
}

bool M5OnScreenKeyboard::loop() {
  _keyCode = 0;
  M5.Lcd.setTextFont(0);
  M5.Lcd.setTextSize(0);
  if (_state == APPEAR && !appear()) return true;

  _msec = millis();

  // draw blink cursor.
  if (useTextbox) M5.Lcd.drawFastVLine(_cursorPos * 6, getY(-1) + textYOffset, 8, (_msec / 150) % 2 ? textboxBackColor : textboxFontColor);

  //if (_msec < _msecNext) return true;
  bool canRepeat = (_msec >= _msecNext);

  bool mod = false;
  bool press = false;
  int oldRepeat = _repeat;
  M5.update();

  // BtnA Pressing Fn _state
  if (M5.BtnA.isPressed()) {
    // 3button simultaneously  switchs morse mode.
    if (M5.BtnB.isPressed() && M5.BtnC.wasPressed()) {
      _fn = true;
      mod = true;
      clearMorse();
      _state = (_state == MORSE) 
             ? LEFTRIGHT
             : MORSE;
      drawKeyboard();
    } else
    if (M5.BtnB.wasPressed()) {
      _fn = true;
      mod = true;
      switchTable();
    } else
    if (M5.BtnC.wasPressed()) {
      _fn = true;
      if (_state == LEFTRIGHT || _state == MORSE) {
        return false; 
      }
      mod = true; 
      _state = LEFTRIGHT;
    } else {
      _fn |= M5.BtnA.pressedFor(msecHold);
    }
  } else if (_fn) {
    if (M5.BtnB.isReleased() && M5.BtnC.isReleased()) _fn = false;
  } else {
    switch (_state) {
    case LEFTRIGHT:   // left right moving.
      if (M5.BtnA.wasReleased()) { --_nowCol; }
      if (M5.BtnB.isPressed()) { press = true; if (M5.BtnB.wasPressed() || canRepeat) { if (++_repeat < COLUMNCOUNT) ++_nowCol; } }
      if (M5.BtnC.wasPressed()) { mod = true; _state = UPDOWN; _repeat = -1; }
      break;
    case UPDOWN:    // up down moving.
      if (M5.BtnA.wasReleased()) { --_nowRow; }
      if (M5.BtnB.isPressed()) { press = true; if (M5.BtnB.wasPressed() || canRepeat) { if (++_repeat < ROWCOUNT) ++_nowRow; } }
      if (M5.BtnC.isPressed()) { press = true; if (M5.BtnC.wasPressed() || canRepeat) { mod = true; ++_repeat; pressKey(); } }
      if (M5.BtnC.wasReleased() && 0 < _repeat) { mod = true; _state = LEFTRIGHT; }
      break;
    case MORSE:    // morse input mode.
      if (M5.BtnB.wasPressed()) { press = true; pressMorse(false); }
      if (M5.BtnC.wasPressed()) { press = true; pressMorse(true ); }
      if (M5.BtnB.releasedFor(1000) && M5.BtnC.releasedFor(1000) ) { inputMorse(); }
      break;
    }
  }
#ifdef _M5PLUSENCODER_H_
  if (PLUSEncoder.update()) {
    switch (_state) {
    case LEFTRIGHT:   // left right moving
      if (PLUSEncoder.wasUp())       { --_nowCol; }
      if (PLUSEncoder.wasDown())     { ++_nowCol; }
      if (PLUSEncoder.wasHold())     { mod = true; switchTable(); break; } 
      if (PLUSEncoder.wasClicked())  { mod = true; _state = UPDOWN; }
      break;
    case UPDOWN:    // up down moving
      if (PLUSEncoder.wasUp())       { --_nowRow; }
      if (PLUSEncoder.wasDown())     { ++_nowRow; }
      if (PLUSEncoder.wasHold())     { mod = true; _state = LEFTRIGHT; }
      if (PLUSEncoder.wasClicked())  { mod = true; pressKey(); _state = LEFTRIGHT; }
      break;
    }
  }
#endif
//#ifndef _M5JOYSTICK_H_
  if (JoyStick.update()) {
    if (JoyStick.isLeft() ) { press = true; if (JoyStick.wasLeft()  || canRepeat) { --_nowCol; ++_repeat; } }
    if (JoyStick.isRight()) { press = true; if (JoyStick.wasRight() || canRepeat) { ++_nowCol; ++_repeat; } }
    if (JoyStick.isUp()   ) { press = true; if (JoyStick.wasUp()    || canRepeat) { --_nowRow; ++_repeat; } }
    if (JoyStick.isDown() ) { press = true; if (JoyStick.wasDown()  || canRepeat) { ++_nowRow; ++_repeat; } }
    if (JoyStick.wasClicked()) { mod = true; pressKey(); }
    if (JoyStick.wasHold()) { mod = true; switchTable(); }
  }
//#endif
  updateButton();
  ButtonDrawer.draw();
  if (_oldCol != _nowCol || _oldRow != _nowRow || _oldTbl != _nowTbl || mod) {
    _nowCol = (_nowCol + COLUMNCOUNT) % COLUMNCOUNT;
    _nowRow = (_nowRow + ROWCOUNT   ) % ROWCOUNT;
    if (_oldTbl != _nowTbl) {
      _oldTbl = _nowTbl;
      drawKeyboard();
    } else {
      drawColumn(_nowCol);
      if (_oldCol != _nowCol) drawColumn(_oldCol);
    }
    _msecNext = _msec + ((canRepeat && 0 < _repeat) ? msecRepeat : msecHold);
    _oldCol = _nowCol;
    _oldRow = _nowRow;
  } else {
    if (!press) {
      _repeat = 0;
      _msecNext = -1;
    }
  }
  return true;
}

void M5OnScreenKeyboard::close() { 
  int y = getY(-1);
  M5.Lcd.fillRect(0, y, M5.Lcd.width(), M5.Lcd.height() - y, 0);
  _state == APPEAR;
  _string = "";
}

int M5OnScreenKeyboard::getX(int col) const { return col * KEYWIDTH; }
int M5OnScreenKeyboard::getY(int row) const { return M5.Lcd.height() - bottomOffset - (ROWCOUNT - row) * keyHeight; }

void M5OnScreenKeyboard::updateButton() {
  if (M5.BtnA.isPressed()) {
    switch (_state) {
    case LEFTRIGHT: ButtonDrawer.setText(_fn?"Fn":"Left", "Panel", M5.BtnB.isPressed() ? "Morse" : "Finish"); break;
    case UPDOWN:    ButtonDrawer.setText(_fn?"Fn":"Up"  , "Panel", M5.BtnB.isPressed() ? "Morse" : "Column"); break;
    case MORSE:     ButtonDrawer.setText(    "Fn"       , "Panel", M5.BtnB.isPressed() ? "Focus" : "Finish"); break;
    }
  } else {
    switch (_state) {
    case LEFTRIGHT: ButtonDrawer.setText("Left/Fn", "Right", "Row"); break;
    case UPDOWN:    ButtonDrawer.setText("Up/Fn"  , "Down" , "Ok" ); break;
    case MORSE:     ButtonDrawer.setText("Fn"     , "."    , "_"  ); break;
    }
  }
}
void M5OnScreenKeyboard::switchTable() {
  _nowTbl = ++_nowTbl % TABLECOUNT;
}

void M5OnScreenKeyboard::pressKey() {
  pressKey(_chartbl[_nowTbl][_nowRow][_nowCol]);
}
void M5OnScreenKeyboard::pressKey(char keycode) {
  _keyCode = keycode;
  if (!useTextbox) return;

  switch (_keyCode) {
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
      _string = _string.substring(0, _cursorPos) + _keyCode + _string.substring(_cursorPos);
      ++_cursorPos;
    }
    break;
  }
  drawTextbox();
}

void M5OnScreenKeyboard::clearMorse() {
  _morseInputBuf = 0;
  _morsePos = 0;
}

void M5OnScreenKeyboard::pressMorse(bool longTone) {
  _morseInputBuf |= (longTone ? 3 : 1) << (_morsePos*2);
  int tbl = (_nowTbl == 2) ? 1 : 0;
  if (++_morsePos > 8) {
    inputMorse(); 
  } else {
    for (int r = 0; r < ROWCOUNT; ++r) {
      for (int c = 0; c < COLUMNCOUNT; ++c) {
        if (_morseInputBuf != _morsetbl[tbl][r][c]) continue;
        _nowRow = r;
        _nowCol = c;
        return;
      }
    }
    _nowRow = ROWCOUNT-1;
    _nowCol = COLUMNCOUNT-1;
  }
}

void M5OnScreenKeyboard::inputMorse() {
  if (!_morseInputBuf) return;
  int tbl = (_nowTbl == 2) ? 1 : 0;

  uint16_t morse = _morseInputBuf;
  clearMorse();
  for (int c = 0; c < COLUMNCOUNT; ++c) {
    for (int r = 0; r < ROWCOUNT; ++r) {
      if (morse != _morsetbl[tbl][r][c]) continue;
      pressKey(_chartbl[_nowTbl][r][c]);
      return;
    }
  }
  tbl = tbl ? 0 : 1;
  for (int c = 0; c < COLUMNCOUNT; ++c) {
    for (int r = 0; r < ROWCOUNT; ++r) {
      if (morse != _morsetbl[tbl][r][c]) continue;
      pressKey(_chartbl[tbl ? 2 : 0][r][c]);
      return;
    }
  }
  _nowRow = ROWCOUNT-1;
  _nowCol = COLUMNCOUNT-1;
}

void M5OnScreenKeyboard::drawKeyTop(int c, int r, int x, int y) {
  int moffset = _state == MORSE ? 2 : 0;
  M5.Lcd.setCursor(x, y + textYOffset + moffset);
  switch (_chartbl[_nowTbl][r][c]) {
  case BS  :  M5.Lcd.print("<B S ");  break;
  case DEL :  M5.Lcd.print(" DEL>");  break;
  case LEFT:  M5.Lcd.print(" <<  ");  break;
  case RIGH:  M5.Lcd.print("  >> ");  break;
  default:
    M5.Lcd.setCursor(x + 13, y + textYOffset + moffset);
    M5.Lcd.print(_chartbl[_nowTbl][r][c]);
    break;
  }
  if (_state == MORSE) {
    int mTbl = (_nowTbl == 2) ? 1 : 0;
    int morse = _morsetbl[mTbl][r][c];
    if (morse != 0 && morse != _chartbl[_nowTbl][r][c]) {
      drawMorse(morse, x + 16 - (numofbits(morse & 0x5555) * 3 + numofbits(morse & 0xAAAA) * 2) / 2, y + moffset);
    }
  }
}

void M5OnScreenKeyboard::drawMorse(uint16_t m, int x, int y)
{
  for (int i = 0; i < 8; ++i) {
    switch (m % 4) {
    case 1: M5.Lcd.drawFastVLine(x, y-1, 3, fontColor); x += 3; break;
    case 3: M5.Lcd.drawFastHLine(x, y  , 3, fontColor); x += 5; break;
    default: return;
    }
    m >>= 2;
  }
}

void M5OnScreenKeyboard::draw() {
  M5.Lcd.setTextFont(0);
  M5.Lcd.setTextSize(0);
  drawKeyboard();
  if (useTextbox) drawTextbox();
}

void M5OnScreenKeyboard::drawTextbox() {
  M5.Lcd.setTextColor(textboxFontColor);
  int y = getY(-1);
  M5.Lcd.setCursor(1, y + textYOffset);
  M5.Lcd.drawFastHLine(0, y, M5.Lcd.width(), frameColor);
  M5.Lcd.fillRect(0, y + 1, M5.Lcd.width(), keyHeight - 1, textboxBackColor);
  M5.Lcd.print(_string);
}

void M5OnScreenKeyboard::drawKeyboard(int h) {
  if (h < 0) h = keyHeight * ROWCOUNT;
  int y = M5.Lcd.height() - bottomOffset - h;
  for (int c = 0; c < COLUMNCOUNT; ++c) {
    int x = getX(c);
    drawColumn(c, x, y, h);
    M5.Lcd.drawFastVLine(x, y, h, frameColor);
  }
  M5.Lcd.drawFastVLine(getX(COLUMNCOUNT), y, h, frameColor);
}

void M5OnScreenKeyboard::drawColumn(int col) {
  drawColumn(col, getX(col), getY(0), ROWCOUNT * keyHeight);
}

void M5OnScreenKeyboard::drawColumn(int col, int x, int y, int h) {
  M5.Lcd.fillRect(x+1, y, KEYWIDTH-1, h, backColor);
  int kh = h / ROWCOUNT;
  if (_nowCol == col) {
    M5.Lcd.fillRect(x+1, y + _nowRow * kh + 1, KEYWIDTH - 1, kh - 1, focusBackColor);
    if (_state == LEFTRIGHT) {
      M5.Lcd.drawRect(x+1, y+1, KEYWIDTH - 1, h - 1, focusBackColor);
      M5.Lcd.drawRect(x+2, y+2, KEYWIDTH - 3, h - 3, focusBackColor);
    }
  }
  for (int r = 0; r < ROWCOUNT; ++r) {
    int tmpy = y + r * kh;
    if (8+textYOffset < y + h - tmpy) {
      M5.Lcd.setTextColor(_nowCol == col && _nowRow == r ? focusFontColor : fontColor);
      drawKeyTop(col, r, x, tmpy);
    }
    M5.Lcd.drawFastHLine(x, tmpy, KEYWIDTH, frameColor);
  }
}

bool M5OnScreenKeyboard::appear() {
  int tmp = (millis() - _msec) / 2;
  if (tmp < keyHeight * ROWCOUNT) {
    drawKeyboard(tmp);
    return false;
  }

  _state = LEFTRIGHT;
  draw();
  return true;
}
