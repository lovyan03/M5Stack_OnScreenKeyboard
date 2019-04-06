#include <M5ButtonDrawer.h>

uint16_t M5ButtonDrawer::frameColor[2] = { 0xA514,0xffff };
uint16_t M5ButtonDrawer::backColor[2]  = { 0x0000,0x0000 };
uint16_t M5ButtonDrawer::fontColor[2]  = { 0xffff,0xffff };
int16_t M5ButtonDrawer::width = 72;
int16_t M5ButtonDrawer::height = 14;
int16_t M5ButtonDrawer::font = 1;
const GFXfont* M5ButtonDrawer::gfxFont = NULL;

void M5ButtonDrawer::setText(const String& btnA, const String& btnB, const String& btnC) {
  setText(0, btnA);
  setText(1, btnB);
  setText(2, btnC);
}
void M5ButtonDrawer::setText(uint8_t idx, const String& str) {
  if (idx < 3 && _titles[idx] != str) {
    _titles[idx] = str;
    _mod[idx] = true;
  }
}

void M5ButtonDrawer::draw(bool force)
{
  if (_mod[0] || force || M5.BtnA.wasPressed() || M5.BtnA.wasReleased()) draw(0, M5.BtnA.isPressed());
  if (_mod[1] || force || M5.BtnB.wasPressed() || M5.BtnB.wasReleased()) draw(1, M5.BtnB.isPressed());
  if (_mod[2] || force || M5.BtnC.wasPressed() || M5.BtnC.wasReleased()) draw(2, M5.BtnC.isPressed());
}

void M5ButtonDrawer::draw(uint8_t idx, bool pressed)
{
  _mod[idx] = false;
  drawButton((idx - 1) * (96 + (84 < width ? (width - 84) / 2 : 0)) + 160, pressed, _titles[idx]);
}

void M5ButtonDrawer::drawButton(int x, bool pressed, const String& title) const
{
  M5.Lcd.setTextSize(1);
  if (gfxFont) {
    M5.Lcd.setFreeFont(gfxFont);
  } else {
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextFont(font);
  }
  M5.Lcd.setTextColor(fontColor[pressed]);
  int16_t fh = M5.Lcd.fontHeight(font);
  if (gfxFont && 12 < fh) fh = fh * 9 / 10;

  int rx = x - width / 2;
  int ry = M5.Lcd.height() - height;
  int rw = width;
  int rh = height;
  int fy = ry + (rh - fh)/2;
  uint16_t color = frameColor[pressed];
  M5.Lcd.drawRect(rx+1,ry  ,rw-2,rh   ,color);
  M5.Lcd.drawRect(rx  ,ry+1,rw  ,rh-2 ,color);

  M5.Lcd.fillRect(rx+2, ry+2, rw-4, rh-4, backColor[pressed]);
  M5.Lcd.drawCentreString(title, x, fy, font);
}

