#include <M5ButtonDrawer.h>

void M5ButtonDrawer::setText(const String& btnA, const String& btnB, const String& btnC) {
  _titles[0] = btnA;
  _titles[1] = btnB;
  _titles[2] = btnC;
}
void M5ButtonDrawer::setText(uint8_t idx, const String& str) {
  if (idx < 3) _titles[idx] = str;
}

void M5ButtonDrawer::draw() const
{
  draw(0, M5.BtnA.isPressed());
  draw(1, M5.BtnB.isPressed());
  draw(2, M5.BtnC.isPressed());
}

void M5ButtonDrawer::draw(uint8_t index, bool pressed) const
{
  draw(index * 96 + 32, pressed, _titles[index]);
}

void M5ButtonDrawer::draw(uint16_t rx, bool pressed, const String& title) const
{
  uint16_t ry = M5.Lcd.height() - height;
  uint16_t rw = width;
  uint16_t rh = height;
  uint16_t color = frameColor[pressed];
  M5.Lcd.drawRect(rx+1,ry  ,rw-2,rh   ,color);
  M5.Lcd.drawRect(rx  ,ry+1,rw  ,rh-2 ,color);
  rx+=2;
  ry+=2;
  rw-=4;
  rh-=4;
  uint16_t h = (rh - 8)/2;
  uint16_t w = title.length() * 6;
  uint16_t x = (rw - w) / 2;
  color = backColor[pressed];
  M5.Lcd.fillRect(rx    , ry, x, rh, color);
  M5.Lcd.fillRect(rx+x+w, ry, rw-w-x, rh, color);
  if (h > 0) {
    M5.Lcd.fillRect(rx+x, ry,       w, h, color);
    M5.Lcd.fillRect(rx+x, ry+rh-h, w, h, color);
  }
  M5.Lcd.setCursor(rx+x, ry + h);
  M5.Lcd.setTextFont(0);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(fontColor[pressed], color);
  M5.Lcd.print(title);
}

