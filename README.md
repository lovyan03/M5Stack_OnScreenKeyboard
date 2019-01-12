M5Stack OnScreenKeyBoard library.
===

Add Keyboard for your M5Stack project.

あなたのM5Stackプロジェクトにキーボードを！

## Description

OnScreenKeyBoard which can be operated with 3 button.  

M5Stack本体の３ボタンで操作できるオンスクリーンキーボード。  

Support FACES KeyBoard and GameBoy unit.  
Support PLUS Encoder unit.  
Support JoyStick unit.  
Support morse code input.  

FACESキーボードとゲームボーイ、PLUSエンコーダユニット、ジョイスティックユニットでも操作可能。  
ABC３ボタン同時押しでモールス入力も使用可能。  

![image](https://user-images.githubusercontent.com/42724151/50738003-86b17980-1212-11e9-9cbf-319bbd885cd9.png)
  
in column select mode:  
 `BtnA click` : The focus move to left.  
 `BtnB click` : The focus move to right.  
 `BtnC click` : Switch to the row selection mode.  
 `BtnA hold and BtnB click` : Keyboard panel switches.  
 `BtnA hold and BtnC click` : Finish keyboard input.  
  
  
in row select mode:  
 `BtnA click` : The focus move to up.  
 `BtnB click` : The focus move to down.  
 `BtnC click` : The focused key is entered, and Switch to the column selection mode.  
 `BtnA hold and BtnB click` : Keyboard panel switches.  
 `BtnA hold and BtnC click` : Switch to the column selection mode.  
  
  
 `BtnA and B hold and BtnC click` : Morse code mode switches.  
![image](https://user-images.githubusercontent.com/42724151/50976020-47e13380-1532-11e9-96dd-d98f09d665f4.png)

in morse code mode:  
 `BtnB click` : Input a short pulse.   
 `BtnC click` : Input a long pulse.  
 `Release BtnB and C for 1 second` : Fix input.  
 `BtnA hold and BtnB click` : Keyboard panel switches.  
 `BtnA hold and BtnC click` : Finish keyboard input.  


 Morse code is GBoard morse compliant.  
[GBoard morse code list](https://gist.github.com/natevw/0fce6b56c606632f8ee780b5d493f94e)

## Usage

```
#include <M5OnScreenKeyboard.h>

M5OnScreenKeyboard m5osk;

  m5osk.useFACES = true;       // FACES unit support.
  m5osk.useJoyStick = true;    // JoyStick unit support.
  m5osk.usePLUSEncoder = true; // PLUS Encoder unit support.

  m5osk.setup("Hello World."); // default text

  while (m5osk.loop()) {
    // You can write your code here.
    delay(1);
  }
  String text = m5osk.getString();
  m5osk.close();
```

## Licence

[LGPL v2.1](https://github.com/lovyan03/M5OnScreenKeyBoard/blob/master/LICENSE)  

## Author

[lovyan03](https://twitter.com/lovyan03)  
