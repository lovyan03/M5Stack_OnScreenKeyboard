#include <M5OnScreenKeyboard.h>
#include <M5Stack.h>

#include <M5ButtonDrawer.h>
#include <M5PLUSEncoder.h>
#include <M5JoyStick.h>

enum 
{ TABLECOUNT  = 4
, ROWCOUNT    = 4
, COLUMNCOUNT =11
, KEYWIDTH   = 29
};

static const char BS   = 0x08;
static const char DEL  = 0x7f;
static const char LEFT = 0x11;
static const char RIGH = 0x13;

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
     }
    , {{0x80, 0x84, 0x88, 0x8c, 0x90, 0x94, 0x98, 0x9c, 0xa0, 0xa4, BS }
     , {0x81, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x99, 0x9d, 0xa1, 0xa5, DEL}
     , {0x82, 0x86, 0x8a, 0x8e, 0x92, 0x96, 0x9a, 0x9e, 0xa2, 0xa6, LEFT}
     , {0x83, 0x87, 0x8b, 0x8f, 0x93, 0x97, 0x9b, 0x9f, 0xa3, 0xa7, RIGH}
    }};

static const PROGMEM uint8_t _morsetbl[TABLECOUNT][ROWCOUNT][COLUMNCOUNT] 
   = {{{0x30, 0x38, 0x3c, 0x3e, 0x3f, 0x2f, 0x27, 0x23, 0x21, 0x20, 0x10 }
     , {0x12, 0x0c, 0x03, 0x0d, 0x02, 0x14, 0x0e, 0x07, 0x08, 0x19, 0}
     , {0x06, 0x0f, 0x0b, 0x1d, 0x09, 0x1f, 0x18, 0x0a, 0x1b, 0x2d, 0}
     , {0x13, 0x16, 0x15, 0x1e, 0x17, 0x05, 0x04, 0x1c, 0x6a, 0x65, 0}
     }
    , {{0x54, 0x6d, 0x25, 0x7b, 0x22, 0x37, 0x61 ,0x5a, 0x7f, 0x63, 0x10}
     , {0x28, 0x50, 0x33, 0x66, 0x32, 0x64, 0x29 ,0x52, 0x2e, 0x2a, 0}
     , {0x72, 0x4a, 0x4c, 0x55, 0x47, 0x73, 0x35 ,0x3d, 0x5e, 0x2d, 0}
     , {0   , 0   , 0   , 0   , 0   , 0   , 0   , 0x1c, 0x6a, 0x65, 0}
     }
    , {{0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0x10}
     , {0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0}
     , {0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0}
     , {0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0}
    }};

static const PROGMEM uint8_t _morsepanel[TABLECOUNT] 
   = {0,0,1,2};

static uint8_t calcMorse(uint8_t m)
{
  uint8_t res = -2;
  bool startbit = false;
  bool flg;
  for (int i = 0; i < 8; ++i) {
    flg = (m & (0x80 >> i));
    if (!startbit) startbit = flg;
    else res += flg ? 3:5;
  }
  return res;
}

void M5OnScreenKeyboard::setString(const String& value) {
  _string = value;
  _cursorPos = _string.length();
}

void M5OnScreenKeyboard::clearString() { 
  _string = "";
  _cursorPos = 0;
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

  if (M5.BtnA.isPressed() || _fn)
  { // BtnA Pressing Fn _state
    if (_fn < 3 && M5.BtnB.isPressed() && M5.BtnC.isPressed())
    { // 3button simultaneously clear string.
      _fn = 3;
      mod = true;
      clearString();
      drawTextbox();
    } else if (_fn < 3 && M5.BtnC.isReleased() && M5.BtnB.wasReleased())
    { // A + B simultaneously switchs morse mode.
      _fn = 3;
      mod = true;
      clearMorse();
      _state = (_state == MORSE) 
             ? LEFTRIGHT
             : MORSE;
      drawKeyboard();
    } else if (_fn < 3 && M5.BtnB.isReleased() && M5.BtnC.wasReleased()) {
      _fn = 3;
      if (_state == LEFTRIGHT || _state == MORSE) {
        return false; 
      }
      mod = true; 
      _state = LEFTRIGHT;
    } else if (M5.BtnB.isReleased() && M5.BtnC.isReleased()) {
      if (M5.BtnA.isReleased()) { // AllBtnReleased clear fn mode
        _fn = 0;
      } else if (_fn == 3) {
        _fn = 2;
      } else if (_fn < 2 && M5.BtnA.pressedFor(msecHold)) {
        _fn = 1;
        press = true; 
        if (canRepeat) {
          switch (_state) {
          case LEFTRIGHT: if (++_repeat < COLUMNCOUNT) --_nowCol; break;
          case UPDOWN:    if (++_repeat < ROWCOUNT)    --_nowRow; break;
          }
        }
      }
    } else if (!_fn) {
      if (M5.BtnB.isPressed() || M5.BtnC.isPressed()) {
        _fn = 1;
      }
    }
  } else {
    if (M5.BtnA.wasReleased()) { switchTable(); }
    switch (_state) {
    case LEFTRIGHT:   // left right moving.
      if (M5.BtnB.isPressed()) { press = true; if (M5.BtnB.wasPressed() || canRepeat) { if (++_repeat < COLUMNCOUNT) ++_nowCol; } }
      if (M5.BtnC.wasPressed()) { mod = true; _state = UPDOWN; _repeat = -1; }
      break;
    case UPDOWN:    // up down moving.
      if (M5.BtnB.isPressed()) { press = true; if (M5.BtnB.wasPressed() || canRepeat) { if (++_repeat < ROWCOUNT) ++_nowRow; } }
      if (M5.BtnC.isPressed()) { press = true; if (M5.BtnC.wasPressed() || canRepeat) { mod = true; ++_repeat; pressKey(); } }
      if (M5.BtnC.wasReleased() && 0 < _repeat) { mod = true; _state = LEFTRIGHT; }
      break;
    case MORSE:    // morse input mode.
      if (M5.BtnB.wasPressed()) { press = true; pressMorse(false); }
      if (M5.BtnC.wasPressed()) { press = true; pressMorse(true ); }
      if (M5.BtnB.releasedFor(msecMorseInput)
       && M5.BtnC.releasedFor(msecMorseInput) ) { inputMorse(); }
      break;
    }
  }
  if (useFACES && Wire.requestFrom(0x08, 1)) {
    while (Wire.available()){
      char key = Wire.read();
      if (key == 0)    { _flgFACESKB = true; continue; }
      if (key == 0xff) { _flgFACESKB = false; continue; }
      press = true;
      if (canRepeat) {
        mod = true;
        ++_repeat;
        if (_flgFACESKB) {
          inputKB(key);
        } else {
          if (!(key & 0x40)) { switchTable(); }
          if (!(key & 0x10)) { pressKey();   }
          if (!(key & 0x20)) { pressKey(BS); } // FACES GameBoy B btn: BS assign.
          if (key >= 0xf0) { // FACES GameBoy cursor
            _nowCol += (0 == (key & 0x08)) ? 1 : (0 == (key & 0x04)) ? -1 : 0;
            _nowRow += (0 == (key & 0x02)) ? 1 : (0 == (key & 0x01)) ? -1 : 0;
          }
        }
      }
    }
  }
  if (useCardKB && Wire.requestFrom(0x5F, 1)) {
    while (Wire.available()){
      char key = Wire.read();
      if (key == 0)  { continue; }
      press = true;
      if (canRepeat) {
        mod = true;
        ++_repeat;
        inputKB(key);
      }
    }
  }
#ifdef _M5PLUSENCODER_H_
  if (usePLUSEncoder && PLUSEncoder.update()) {
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
  if (useJoyStick && JoyStick.update()) {
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
    _msecNext = _msec + ((canRepeat && 1 < _repeat) ? msecRepeat : msecHold);
    _oldCol = _nowCol;
    _oldRow = _nowRow;
  } else {
    if (!press) {
      _repeat = 0;
      _msecNext = 0;
    }
  }
  return true;
}
void M5OnScreenKeyboard::close() { 
  int y = getY(-1);
  M5.Lcd.fillRect(0, y, M5.Lcd.width(), M5.Lcd.height() - y, 0);
  _state == APPEAR;
  clearString();
}

int M5OnScreenKeyboard::getX(int col) const { return col * KEYWIDTH; }
int M5OnScreenKeyboard::getY(int row) const { return M5.Lcd.height() - bottomOffset - (ROWCOUNT - row) * keyHeight; }

void M5OnScreenKeyboard::updateButton() {
  if (M5.BtnA.isPressed() || _fn) {
    switch (_state) {
    case LEFTRIGHT: ButtonDrawer.setText(_fn==1?"Left":_fn?"Fn":"Panel", "Morse", "Finish"); break;
    case UPDOWN:    ButtonDrawer.setText(_fn==1?"Up"  :_fn?"Fn":"Panel", "Morse", "Column"); break;
    case MORSE:     ButtonDrawer.setText("Fn", "Focus", "Finish"); break;
    }
    if (M5.BtnB.isPressed()) ButtonDrawer.setText(2, "AllClear");
  } else {
    switch (_state) {
    case LEFTRIGHT: ButtonDrawer.setText("Panel/Left" , "Right", "Row"); break;
    case UPDOWN:    ButtonDrawer.setText("Panel/Up"   , "Down" , "Ok" ); break;
    case MORSE:     ButtonDrawer.setText("Panel/Fn", "."    , "_"  ); break;
    }
  }
}
void M5OnScreenKeyboard::switchTable() {
  _nowTbl = ++_nowTbl % (TABLECOUNT - (useOver0x80Chars?0:1));
}

void M5OnScreenKeyboard::inputKB(char key)
{
  switch (key) {
  case 0x81: pressKey(LEFT); break;
  case 0x83: pressKey(RIGH); break;
  case BS:   pressKey(key);  break;
  default:
    if (0x20 <= key && key < 0x80) {
      pressKey(key);
    }
    break;
  }
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
}

void M5OnScreenKeyboard::pressMorse(bool longTone) {
  _morseInputBuf = (0 == _morseInputBuf ? 2 : (_morseInputBuf << 1)) | (longTone ? 0 : 1);
  if (_morseInputBuf & 0x80) {
    inputMorse(); 
  } else {
    int mp = _morsepanel[_nowTbl];
    for (int r = 0; r < ROWCOUNT; ++r) {
      for (int c = 0; c < COLUMNCOUNT; ++c) {
        if (_morseInputBuf != _morsetbl[mp][r][c]) continue;
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

  uint16_t morse = _morseInputBuf;
  clearMorse();
  int mp = _morsepanel[_nowTbl];
  for (int c = 0; c < COLUMNCOUNT; ++c) {
    for (int r = 0; r < ROWCOUNT; ++r) {
      if (morse != _morsetbl[mp][r][c]) continue;
      pressKey(_chartbl[_nowTbl][r][c]);
      return;
    }
  }
  for (int c = 0; c < COLUMNCOUNT; ++c) {
    for (int r = 0; r < ROWCOUNT; ++r) {
      int m = -1;
      for (int t = 0; t < TABLECOUNT; ++t) {
        if (m == _morsepanel[t] || mp == _morsepanel[t]) continue;
        m = _morsepanel[t];
        if (morse != _morsetbl[m][r][c]) continue;
        pressKey(_chartbl[t][r][c]);
        return;
      }
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
    int morse = _morsetbl[_morsepanel[_nowTbl]][r][c];
    if (morse != 0) {
      drawMorse(morse, x + 15 - calcMorse(morse) / 2, y + moffset);
    }
  }
}

void M5OnScreenKeyboard::drawMorse(uint8_t m, int x, int y)
{
  bool startbit = false;
  bool flg;
  for (int i = 0; i < 8; ++i) {
    flg = (m & (0x80 >> i));
    if (!startbit) startbit = flg;
    else {
      if (flg) M5.Lcd.drawFastVLine(x, y-1, 3, fontColor);
      else     M5.Lcd.drawFastHLine(x, y  , 3, fontColor);
      x += flg ? 3 : 5;
    }
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
