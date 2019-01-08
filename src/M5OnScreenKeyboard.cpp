#include <M5OnScreenKeyboard.h>
#include <M5Stack.h>

#include <M5ButtonDrawer.h>
#include <M5PLUSEncoder.h>

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
  _string = value; drawTextbox();
  _cursorPos = _string.length();
}

void M5OnScreenKeyboard::setup(const String& value) {
  setString(value);

  drawKeyboard();
  drawTextbox();
  _nowTbl = 0;
  _nowCol = 0;
  _nowRow = 0;
  _state = LEFTRIGHT;
  _msecNext = 0;
}

bool M5OnScreenKeyboard::loop() {
  uint32_t msec = millis();

  // draw blink cursor.
  M5.Lcd.drawFastVLine(_cursorPos * 6, getY(-1) + textYOffset, 8, (msec / 150) % 2 ? editBackColor : editFontColor);

  //if (msec < _msecNext) return true;
  bool canRepeat = (msec >= _msecNext);

  bool mod = false;
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
      if (_state == LEFTRIGHT) return false;
      mod = true; 
      _state = LEFTRIGHT;
    }
    _fn |= M5.BtnA.pressedFor(msecHold);
  } else {
    switch (_state) {
    case LEFTRIGHT:   // left right moving
      if (M5.BtnA.wasReleased() && !_fn) { --_nowCol; }
      if (M5.BtnB.wasPressed() || (M5.BtnB.isPressed() && canRepeat)) { if (++_repeat < COLUMNCOUNT) ++_nowCol; }
      if (M5.BtnC.wasPressed()) { mod = true; _state = UPDOWN; _repeat = -1; }
      break;
    case UPDOWN:    // up down moving
      if (M5.BtnA.wasReleased() && !_fn) { --_nowRow; }
      if (M5.BtnB.wasPressed() || (M5.BtnB.isPressed() && canRepeat)) { if (++_repeat < ROWCOUNT) ++_nowRow; }
      if (M5.BtnC.wasPressed() || (M5.BtnC.isPressed() && canRepeat)) { mod = true; ++_repeat; pressKey(); }
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
  updateButton();
  ButtonDrawer.draw();
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
    _msecNext = msec + ((canRepeat && 0 < _repeat) ? msecRepeat : msecHold);
    _oldCol = _nowCol;
    _oldRow = _nowRow;
  } else {
    if (M5.BtnB.isReleased() && M5.BtnC.isReleased()) {
      _repeat = 0;
      _msecNext = -1;
    }
  }
  return true;
}

void M5OnScreenKeyboard::close() { 
  int y = getY(-1);
  M5.Lcd.fillRect(0, y, TFT_HEIGHT, TFT_WIDTH - y, 0);
  _string = "";
}

int M5OnScreenKeyboard::getX(int col) const { return col * KEYWIDTH; }
int M5OnScreenKeyboard::getY(int row) const { return TFT_WIDTH - bottomOffset - (ROWCOUNT - row) * keyHeight; }

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
}

void M5OnScreenKeyboard::drawKeyTop(int c, int r, int x, int y) {
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

void M5OnScreenKeyboard::drawTextbox() {
  M5.Lcd.setTextColor(editFontColor);
  int y = getY(-1);
  M5.Lcd.setCursor(1, y + textYOffset);
  M5.Lcd.drawFastHLine(0, y, TFT_HEIGHT, frameColor);
  M5.Lcd.fillRect(0, y + 1, TFT_HEIGHT, keyHeight - 1, editBackColor);
  M5.Lcd.print(_string);
}

void M5OnScreenKeyboard::drawKeyboard() {
  int y = getY(0);
  int h = getY(ROWCOUNT) - y;
  for (int c = 0; c < COLUMNCOUNT; c++) {
    int x = getX(c);
    drawKeyboard(c, x, y, h);
    M5.Lcd.drawFastVLine(x, y, h, frameColor);
  }
  M5.Lcd.drawFastVLine(getX(COLUMNCOUNT), y, h, frameColor);
}

void M5OnScreenKeyboard::drawKeyboard(int col) {
  drawKeyboard(col, getX(col), getY(0), ROWCOUNT * keyHeight);
}

void M5OnScreenKeyboard::drawKeyboard(int col, int x, int y, int h) {
  M5.Lcd.fillRect(x+1, y, KEYWIDTH-1, h, backColor);
  if (_nowCol == col) {
    M5.Lcd.fillRect(x+1, getY(_nowRow)+1, KEYWIDTH - 1, keyHeight - 1, focusBackColor);
    if (_state == LEFTRIGHT) {
      M5.Lcd.drawRect(x+1, y+1, KEYWIDTH - 1, h - 1, focusBackColor);
      M5.Lcd.drawRect(x+2, y+2, KEYWIDTH - 3, h - 3, focusBackColor);
    }
  }
  for (int r = 0; r < ROWCOUNT; r++) {
    y = getY(r);
    M5.Lcd.setTextColor(_nowCol == col && _nowRow == r ? focusFontColor : fontColor);
    drawKeyTop(col, r, x, y);
    M5.Lcd.drawFastHLine(x, y, KEYWIDTH, frameColor);
  }
}

