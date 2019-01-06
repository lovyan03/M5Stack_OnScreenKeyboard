
#include <M5Stack.h>
#include <M5OnScreenKeyboard.h>

M5OnScreenKeyboard m5osk;

void setup() {
  M5.begin();
  m5osk.setup("Hello World.");
}
void loop() {
  while (m5osk.loop()) delay(1);

  M5.Lcd.clear(0);
  M5.Lcd.setCursor(1,100);
  M5.Lcd.print(m5osk.getString());

  delay(1000);
  m5osk.setup(m5osk.getString());
}