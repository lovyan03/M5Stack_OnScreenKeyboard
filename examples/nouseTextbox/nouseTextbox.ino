#include <M5Stack.h>
#include <M5OnScreenKeyboard.h>


M5OnScreenKeyboard m5osk;

void setup() {
  M5.begin();
  Wire.begin();

// textbox disabled.
  m5osk.useTextbox = false;

  m5osk.useFACES = true;       // FACES unit support.
  m5osk.useCardKB = true;      // CARDKB unit support.
  m5osk.useJoyStick = true;    // JoyStick unit support.
  m5osk.usePLUSEncoder = true; // PLUS Encoder unit support.
  m5osk.useFACESEncoder = true;// FACES Encoder unit support.

}
void loop() {
  String text;

  m5osk.setup();
  
  while (m5osk.loop()) {

    M5.Lcd.setCursor(0,0);
    M5.Lcd.setTextColor(0xfff0, 0);
    M5.Lcd.setTextFont(4);
    M5.Lcd.setTextSize(1);
    M5.Lcd.printf("msec:%09d", millis());

    // get pressed character code.
    char key = m5osk.getKeyCode();

    switch (key) {
      case 0:     // no input.
      case 0x7f:  // DEL.
      case 0x1d:  // LEFT.
      case 0x1c:  // RIGHT.
        break;

      case 0x08: // backspace
        text = text.substring(0,text.length() - 1); 
        M5.Lcd.clear(0);
        M5.Lcd.setCursor(0, 40);
        M5.Lcd.print(text);
        m5osk.draw();    // force redraw.
        break;

      default: 
        text += key;
        M5.Lcd.setCursor(0, 40);
        M5.Lcd.print(text);
        m5osk.draw();    // force redraw.
        break;
    }
  }

  m5osk.close();

  delay(2000);
  M5.Lcd.clear(0);
}