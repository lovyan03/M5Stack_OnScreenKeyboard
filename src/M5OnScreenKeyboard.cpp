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
    if (M5.BtnB.wasPressed()) {
      _fn = true;
      mod = true;
      switchTable();
    } else
    if (M5.BtnC.wasPressed()) {
      _fn = true;
      if (_state == LEFTRIGHT) {
        return false; 
      }
      mod = true; 
      _state = LEFTRIGHT;
    }
    _fn |= M5.BtnA.pressedFor(msecHold);
  } else {
    switch (_state) {
    case LEFTRIGHT:   // left right moving
      if (M5.BtnA.wasReleased() && !_fn) { --_nowCol; }
      if (M5.BtnB.isPressed()) { press = true; if (M5.BtnB.wasPressed() || canRepeat) { if (++_repeat < COLUMNCOUNT) ++_nowCol; } }
      if (M5.BtnC.wasPressed()) { mod = true; _state = UPDOWN; _repeat = -1; }
      break;
    case UPDOWN:    // up down moving
      if (M5.BtnA.wasReleased() && !_fn) { --_nowRow; }
      if (M5.BtnB.isPressed()) { press = true; if (M5.BtnB.wasPressed() || canRepeat) { if (++_repeat < ROWCOUNT) ++_nowRow; } }
      if (M5.BtnC.isPressed()) { press = true; if (M5.BtnC.wasPressed() || canRepeat) { mod = true; ++_repeat; pressKey(); } }
      if (M5.BtnC.wasReleased() && 0 < _repeat) { mod = true; _state = LEFTRIGHT; }

      break;
    }
    _fn = false;
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
    case LEFTRIGHT: ButtonDrawer.setText(_fn?"Fn":"Left", "Panel", "Finish"); break;
    case UPDOWN:    ButtonDrawer.setText(_fn?"Fn":"Up", "Panel", "Column"); break;
    }
  } else {
    switch (_state) {
    case LEFTRIGHT: ButtonDrawer.setText("Left/Fn", "Right", "Row"); break;
    case UPDOWN:    ButtonDrawer.setText("Up/Fn", "Down", "Ok"); break;
    }
  }
}
void M5OnScreenKeyboard::switchTable() {
  _nowTbl = ++_nowTbl % TABLECOUNT;
}

void M5OnScreenKeyboard::pressKey() {
  _keyCode = _chartbl[_nowTbl][_nowRow][_nowCol];
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
      _string = _string.substring(0, _cursorPos) + _chartbl[_nowTbl][_nowRow][_nowCol] + _string.substring(_cursorPos);
      ++_cursorPos;
    }
    break;
  }
  drawTextbox();
}

void M5OnScreenKeyboard::drawKeyTop(int c, int r, int x, int y) {
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
    if (10 < y + h - tmpy) {
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
