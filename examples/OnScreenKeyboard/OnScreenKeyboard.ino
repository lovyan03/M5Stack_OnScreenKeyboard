
#include <M5Stack.h>
#include <M5OnScreenKeyboard.h>

M5OnScreenKeyboard m5osk;

void setup() {
  M5.begin();

/* // color change example.
  m5osk.fontColor   = 0x0208;
  m5osk.backColor   = 0xFFFF;
  m5osk.frameColor  = 0x0208;
  m5osk.focusFontColor = 0xFFFF;
  m5osk.focusBackColor = 0x4210;
  m5osk.editFontColor = 0xFFFF;
  m5osk.editBackColor = 0xC618;
//*/

/* // response speed change example.
  m5osk.msecHold = 200;
  m5osk.msecRepeat= 100;
//*/

  m5osk.setup("Hello World.");
}
void loop() {
  while (m5osk.loop()) delay(1);
  String text = m5osk.getString();
  m5osk.close();

  M5.Lcd.setCursor(1,100);
  M5.Lcd.print(text);

  delay(2000);
  M5.Lcd.clear(0);

  m5osk.setup(text);
}