#include <M5Stack.h>
#include <M5OnScreenKeyboard.h>
//#include <M5JoyStick.h>

M5OnScreenKeyboard m5osk;

void setup() {
  M5.begin();
  Wire.begin();

  m5osk.useFACES = true;       // FACES unit support.
  m5osk.useJoyStick = true;    // JoyStick unit support.
  m5osk.usePLUSEncoder = true; // PLUS Encoder unit support.

/* // color change example.
  m5osk.fontColor   = 0x0208;
  m5osk.backColor   = 0xFFFF;
  m5osk.frameColor  = 0x0208;
  m5osk.focusFontColor = 0xFFFF;
  m5osk.focusBackColor = 0x4210;
  m5osk.textboxFontColor = 0xFFFF;
  m5osk.textboxBackColor = 0xC618;
//*/

/* // response speed change example.
  m5osk.msecHold = 200;
  m5osk.msecRepeat= 100;
//*/

/* // JoyStick unit rotation setting
//  need #include <M5JoyStick.h>
JoyStick.setRotate(0);   // 0~3 rotation setting.
//*/

  m5osk.setup("Hello World.");
}
void loop() {

  while (m5osk.loop()) {
    // You can write your code here.
    delay(1);
  }

  // Get input string.
  String text = m5osk.getString();
  m5osk.close();

  M5.Lcd.setCursor(1,100);
  M5.Lcd.print(text);

  delay(2000);
  M5.Lcd.clear(0);

  m5osk.setup(text);
}