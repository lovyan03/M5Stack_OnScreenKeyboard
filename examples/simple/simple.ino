#include <M5Stack.h>
#include <M5OnScreenKeyboard.h>
//#include <M5JoyStick.h>
#include <M5ButtonDrawer.h>

M5OnScreenKeyboard m5osk;

void setup() {
  M5.begin();
  Wire.begin();
 
  m5osk.useFACES = true;       // FACES unit support.
  m5osk.useCardKB = true;      // CardKB unit support.
  m5osk.useJoyStick = true;    // JoyStick unit support.
  m5osk.usePLUSEncoder = true; // PLUS Encoder unit support.

/*
 // style change example.
  m5osk.fontColor[0] = 0x0000;
  m5osk.fontColor[1] = 0xFFFF;
  m5osk.backColor[0] = 0xF79E;
  m5osk.backColor[1] = 0x8410;
  m5osk.frameColor[0]  = 0x4208;
  m5osk.frameColor[1]  = 0xFFFF;
  m5osk.textboxFontColor = 0xFFFF;
  m5osk.textboxBackColor = 0x8410;
  m5osk.keyHeight = 20;
  m5osk.font = 2;

  M5ButtonDrawer::fontColor[0] = 0xFFFF;
  M5ButtonDrawer::fontColor[1] = 0x0000;
  M5ButtonDrawer::backColor[0] = 0x0010;
  M5ButtonDrawer::backColor[1] = 0xF79E;
  M5ButtonDrawer::width = 90;
  M5ButtonDrawer::height = 20;
  M5ButtonDrawer::font = 2;
//*/

/* // response speed change example.
  m5osk.msecHold = 200;
  m5osk.msecRepeat= 100;
  m5osk.msecMorseInput = 500;
//*/

/* // JoyStick unit rotation setting
//  need #include <M5JoyStick.h>
JoyStick.setRotate(0);   // 0~3 rotation setting.
//*/

/* // use non-latin-chars (tentative)
  m5osk.useOver0x80Chars = true;
//*/

  m5osk.setup();
//m5osk.setup("Hello World."); // You can also set default text
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