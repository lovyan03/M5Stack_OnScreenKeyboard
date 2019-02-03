#include <M5OnScreenKeyboard.h>

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
  _btnDrawer.setText("","","");
  _btnDrawer.draw(true);
  setString(value);
  _tbl = 0;
  _col = 0;
  _row = 0;
  _state = APPEAR;
  _msecLast = 0;
  _msec = millis();
}

bool M5OnScreenKeyboard::loop() {
  _keyCode = 0;
  M5.Lcd.setTextSize(1);
  if (_state == APPEAR && !appear()) return true;

  _msec = millis();
  M5.update();

  eState oldState = _state;
  int8_t oldTbl = _tbl;
  int8_t oldCol = _col;
  int8_t oldRow = _row;
  int oldRepeat = _repeat;
  bool canRepeat = _repeat == 0 || (_msec - _msecLast) >= (1 < _repeat ? msecRepeat : msecHold);

  bool press = false;

  if (M5.BtnA.isPressed() || _fn)
  { // BtnA Pressing Fn _state
    if (_fn < 3 && M5.BtnB.isPressed() && M5.BtnC.isPressed())
    { // 3button simultaneously: clear string.
      _fn = 3;
      clearString();
      drawTextbox();
    } else if (_fn < 3 && M5.BtnC.isReleased() && M5.BtnB.wasReleased())
    { // A + B simultaneously: switchs morse mode.
      _fn = 3;
      clearMorse();
      _state = (_state == MORSE)
             ? LEFTRIGHT
             : MORSE;
    } else if (_fn < 3 && M5.BtnB.isReleased() && M5.BtnC.wasReleased()) {
      _fn = 3;
      if (_state == LEFTRIGHT || _state == MORSE)
      { // A + C simultaneously: finish input.
        while (M5.BtnA.isPressed()) M5.update();
        return false; 
      }
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
          case LEFTRIGHT: if (++_repeat < COLUMNCOUNT) --_col; break;
          case UPDOWN:    if (++_repeat < ROWCOUNT)    --_row; break;
          }
        }
      }
    } else if (!_fn) {
      // simultaneously: fn mode.
      _fn = (M5.BtnB.isPressed() || M5.BtnC.isPressed()) ? 1 : 0;
    }
  } else {
    if (M5.BtnA.wasReleased()) { switchTable(); }
    bool btnB = M5.BtnB.isPressed();
    bool btnC = M5.BtnC.isPressed();
    press |= btnB || btnC;
    switch (_state) {
    case LEFTRIGHT:   // left right moving.
      if (btnB && canRepeat) { if (++_repeat < COLUMNCOUNT) ++_col; }
      if (M5.BtnC.wasReleased()) { _state = UPDOWN; _repeat = 0; }
      break;
    case UPDOWN:    // up down moving.
      if (btnB && canRepeat) { if (++_repeat < ROWCOUNT) ++_row; }
      if (btnC && canRepeat) { ++_repeat; pressKey(); }
      if (M5.BtnC.wasReleased() && 0 < _repeat) { _state = LEFTRIGHT; }
      break;
    case MORSE:    // morse input mode.
      if (M5.BtnB.wasPressed() ) { pressMorse(false); }
      if (M5.BtnC.wasPressed()) { pressMorse(true ); }
      if (M5.BtnB.releasedFor(msecMorseInput)
       && M5.BtnC.releasedFor(msecMorseInput)
       && _morseInputBuf) { ++_repeat; inputMorse(); }
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
        ++_repeat;
        if (_flgFACESKB) {
          inputKB(key);
        } else {
          if (!(key & 0x40)) { switchTable(); }
          if (!(key & 0x10)) { pressKey();   }
          if (!(key & 0x20)) { pressKey(BS); } // FACES GameBoy B btn: BS assign.
          _col += (0 == (key & 0x08)) ? 1 : (0 == (key & 0x04)) ? -1 : 0;
          _row += (0 == (key & 0x02)) ? 1 : (0 == (key & 0x01)) ? -1 : 0;
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
        ++_repeat;
        inputKB(key);
      }
    }
  }
#ifdef _M5PLUSENCODER_H_
  if (usePLUSEncoder && PLUSEncoder.update()) {
    switch (_state) {
    case LEFTRIGHT:   // left right moving
      if (PLUSEncoder.wasUp())       { --_col; }
      if (PLUSEncoder.wasDown())     { ++_col; }
      if (PLUSEncoder.wasHold())     { switchTable(); break; } 
      if (PLUSEncoder.wasClicked())  { _state = UPDOWN; }
      break;
    case UPDOWN:    // up down moving
      if (PLUSEncoder.wasUp())       { --_row; }
      if (PLUSEncoder.wasDown())     { ++_row; }
      if (PLUSEncoder.wasHold())     { _state = LEFTRIGHT; }
      if (PLUSEncoder.wasClicked())  { ++_repeat; pressKey(); _state = LEFTRIGHT; }
      break;
    }
  }
#endif
//#ifndef _M5JOYSTICK_H_
  if (useJoyStick && JoyStick.update()) {
    if (!JoyStick.isNeutral()) {
      press = true;
      if (canRepeat) {
        ++_repeat;
        if (JoyStick.isLeft() ) { --_col; }
        if (JoyStick.isRight()) { ++_col; }
        if (JoyStick.isUp()   ) { --_row; }
        if (JoyStick.isDown() ) { ++_row; }
      }
    }
    if (JoyStick.wasClicked()) { ++_repeat; pressKey(); }
    if (JoyStick.wasHold()) { switchTable(); }
  }
//#endif
  updateButton();
  _btnDrawer.draw();
  if (oldCol != _col
   || oldRow != _row
   || oldTbl != _tbl
   || oldState != _state
   || oldRepeat != _repeat
     ) {
    _col = (_col + COLUMNCOUNT) % COLUMNCOUNT;
    _row = (_row + ROWCOUNT   ) % ROWCOUNT;
    if (oldTbl != _tbl || oldState != _state) {
      drawKeyboard();
    } else {
      drawColumn(_col);
      if (oldCol != _col) drawColumn(oldCol);
    }
    _msecLast = _msec;
  } else {
    if (!press) {
      _repeat = 0;
      if (_pressed != 0 && (_msec - _msecLast) >= msecHold) {
        _pressed = 0;
        drawColumn(_col);
      }
    }
  }
  // draw blink cursor.
  if (useTextbox) {
    int x = M5.Lcd.textWidth(_string.substring(0, _cursorPos), font);
    M5.Lcd.drawFastVLine( x
                        , getY(-1) + (keyHeight - M5.Lcd.fontHeight(font)) / 2
                        , M5.Lcd.fontHeight(font)
                        , (_msec / 150) % 2 ? textboxBackColor : textboxFontColor);
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
int M5OnScreenKeyboard::getY(int row) const { return M5.Lcd.height() - M5ButtonDrawer::height - (ROWCOUNT - row) * keyHeight; }

void M5OnScreenKeyboard::updateButton() {
  if (M5.BtnA.isPressed() || _fn) {
    bool btnB = M5.BtnB.isPressed();
    _btnDrawer.setText(1, _state == MORSE ? "Focus" : "Morse");
    if (btnB) _btnDrawer.setText(2, "AllClear");
    switch (_state) {
    case LEFTRIGHT:
      _btnDrawer.setText(0, _fn==1?"Left":_fn?"Fn":"Panel");
      if (!btnB) _btnDrawer.setText(2, "Finish");
      break;

    case UPDOWN:
      _btnDrawer.setText(0, _fn==1?"Up"  :_fn?"Fn":"Panel");
      if (!btnB) _btnDrawer.setText(2, "Column");
      break;

    case MORSE:
      _btnDrawer.setText(0, "Fn");
      if (!btnB) _btnDrawer.setText(2, "Finish");
      break;
    }
  } else {
    switch (_state) {
    case LEFTRIGHT: _btnDrawer.setText("Panel/Left" , "Right", "Row"); break;
    case UPDOWN:    _btnDrawer.setText("Panel/Up"   , "Down" , "Ok" ); break;
    case MORSE:     _btnDrawer.setText("Panel/Fn", "."    , "_"  ); break;
    }
  }
}
void M5OnScreenKeyboard::switchTable() {
  _tbl = ++_tbl % (TABLECOUNT - (useOver0x80Chars?0:1));
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
  pressKey(_chartbl[_tbl][_row][_col]);
}
void M5OnScreenKeyboard::pressKey(char keycode) {
  _pressed = keycode;
  _keyCode = keycode;
  if (!useTextbox) return;

  switch (_keyCode) {
    case BS:
      if (0 < _cursorPos) {
        _string = _string.substring(0, _cursorPos-1) + _string.substring(_cursorPos);
        --_cursorPos;
      }
      break;

    case DEL:
      if (_cursorPos < _string.length()) {
        _string = _string.substring(0, _cursorPos) + _string.substring(_cursorPos+1);
      }
      break;

    case LEFT:
      _cursorPos -= (0 < _cursorPos ? 1 : 0);
      break;

    case RIGH:
      _cursorPos += (_string.length() > _cursorPos ? 1 : 0);
      break;

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
    int mp = _morsepanel[_tbl];
    for (int r = 0; r < ROWCOUNT; ++r) {
      for (int c = 0; c < COLUMNCOUNT; ++c) {
        if (_morseInputBuf != _morsetbl[mp][r][c]) continue;
        _row = r;
        _col = c;
        return;
      }
    }
    _row = ROWCOUNT-1;
    _col = COLUMNCOUNT-1;
  }
}

void M5OnScreenKeyboard::inputMorse() {
  if (!_morseInputBuf) return;

  uint16_t morse = _morseInputBuf;
  clearMorse();
  int mp = _morsepanel[_tbl];
  for (int c = 0; c < COLUMNCOUNT; ++c) {
    for (int r = 0; r < ROWCOUNT; ++r) {
      if (morse != _morsetbl[mp][r][c]) continue;
      pressKey(_chartbl[_tbl][r][c]);
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
  _row = ROWCOUNT-1;
  _col = COLUMNCOUNT-1;
}

void M5OnScreenKeyboard::drawKeyTop(int c, int r, int x, int y, int kh)
{
  int fh = M5.Lcd.fontHeight(font);
  int moffset = _state == MORSE ? 2 : 0;

  char tbl[2] = {_chartbl[_tbl][r][c],0};
  char* str = tbl;
  char code = _chartbl[_tbl][r][c];
  switch (code) {
  case BS  :  str = "BS";  break;
  case DEL :  str = "DEL";  break;
  case LEFT:  str = "<<";  break;
  case RIGH:  str = ">>";  break;
  }
  uint16_t color = fontColor[_col == c && _row == r ? 1 : 0];
  int fy = min(y + (kh - fh + 1) / 2 + moffset, M5.Lcd.height() - M5ButtonDrawer::height - fh);
  M5.Lcd.setTextColor(color);
  M5.Lcd.drawCentreString(str, x + 16, fy, font);
  if (_state == MORSE) {
    int morse = _morsetbl[_morsepanel[_tbl]][r][c];
    if (morse != 0) {
      drawMorse(morse, x + 15 - calcMorse(morse) / 2, y + moffset, color);
    }
  }
  if (_pressed == code) {
    M5.Lcd.drawRect(x+1, y+1, KEYWIDTH - 2, keyHeight - 1, frameColor[1]);
    _pressed = -1;
  }
}

void M5OnScreenKeyboard::drawMorse(uint8_t m, int x, int y, uint16_t color)
{
  bool startbit = false;
  bool flg;
  for (int i = 0; i < 8; ++i) {
    flg = (m & (0x80 >> i));
    if (!startbit) startbit = flg;
    else {
      if (flg) M5.Lcd.drawFastVLine(x, y-1, 3, color);
      else     M5.Lcd.drawFastHLine(x, y  , 3, color);
      x += flg ? 3 : 5;
    }
  }
}

void M5OnScreenKeyboard::draw() {
  M5.Lcd.setTextSize(1);
  drawKeyboard();
  if (useTextbox) drawTextbox();
}

void M5OnScreenKeyboard::drawTextbox() {
  int y = getY(-1);
  M5.Lcd.setTextColor(textboxFontColor);
  M5.Lcd.drawFastHLine(0, y, M5.Lcd.width(), frameColor[0]);
  M5.Lcd.fillRect(0, y + 1, M5.Lcd.width(), keyHeight - 1, textboxBackColor);
  M5.Lcd.drawString(_string, 1, y + (keyHeight - M5.Lcd.fontHeight(font)) / 2, font);
}

void M5OnScreenKeyboard::drawKeyboard(int h) {
  if (h < 0) h = keyHeight * ROWCOUNT;
  int y = M5.Lcd.height() - M5ButtonDrawer::height - h;
  for (int c = 0; c < COLUMNCOUNT; ++c) {
    int x = getX(c);
    drawColumn(c, x, y, h);
    M5.Lcd.drawFastVLine(x, y, h, frameColor[0]);
  }
  M5.Lcd.drawFastVLine(getX(COLUMNCOUNT), y, h, frameColor[0]);
}

void M5OnScreenKeyboard::drawColumn(int col) {
  drawColumn(col, getX(col), getY(0), ROWCOUNT * keyHeight);
}

void M5OnScreenKeyboard::drawColumn(int col, int x, int y, int h) {
  M5.Lcd.fillRect(x+1, y, KEYWIDTH-1, h, backColor[0]);
  int kh = h / ROWCOUNT;
  if (_col == col) {
    M5.Lcd.fillRect(x+1, y + _row * kh + 1, KEYWIDTH - 1, kh - 1, backColor[1]);
    if (_state == LEFTRIGHT) {
      M5.Lcd.drawRect(x+1, y+1, KEYWIDTH - 1, h - 1, backColor[1]);
      M5.Lcd.drawRect(x+2, y+2, KEYWIDTH - 3, h - 3, backColor[1]);
    }
  }
  for (int r = 0; r < ROWCOUNT; ++r) {
    int tmpy = y + r * kh;
    drawKeyTop(col, r, x, tmpy, kh);
    M5.Lcd.drawFastHLine(x, tmpy, KEYWIDTH, frameColor[0]);
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
