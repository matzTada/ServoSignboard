/*
  Servo(s) control via Serial, Button for caps of perfume bottles
  movie: https://youtu.be/TcauUesZbas
  modified 2017/July/28
  by Tada Matz
*/

#include <Servo.h>

class ButtonServo : public Servo {
  public:
    int buttonPin;
    int pastButtonState;
    int servoPin;
    int servoVal;
    void registerPin(int _buttonPin, int _servoPin) {
      buttonPin = _buttonPin;
      pastButtonState = HIGH;
      servoPin = _servoPin;
      servoVal = 0;
      if (buttonPin >= 0) pinMode(buttonPin, INPUT_PULLUP);
      attach(servoPin, 800, 180); // parameters for GWS 2BBMG ref: http://tetsuakibaba.jp/index.php?page=workshop/ServoBasis/main
    }
    boolean isButtonPushed() {
      if (buttonPin >= 0) if (digitalRead(buttonPin) == LOW) return true;
        else return false;
    }
    boolean isButtonClicked() {
      int currentButtonState;
      if (buttonPin >= 0) currentButtonState = digitalRead(buttonPin);
      if (pastButtonState == HIGH && currentButtonState == LOW) servoVal = 180 - servoVal;
      pastButtonState = currentButtonState;
    }
    void rotateByPush() {
      if (isButtonPushed()) write(180);
      else write(0);
    }
    void rotateByClick() {
      isButtonClicked();
      write(servoVal);
    }
};

ButtonServo bss[4];

void rotateBySerial() { // example command : S1180 means that the servo whose id = 1 rotates 180 degrees
  if (Serial.available() > 0) {
    String inputStr = Serial.readStringUntil("\n");
    if (inputStr.charAt(0) == 'S') {
      if (inputStr.charAt(1) == 'A') {
        int val = inputStr.substring(2, 5).toInt();
        val = constrain(val, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
        for (int i = 0; i < sizeof(bss) / sizeof(bss[0]); i++) {
          bss[i].write(val);                  // sets the servo position according to the scaled value}
        }
      } else {
        int servoId = (int)(inputStr.charAt(1) - '0');
        if (0 <= servoId && servoId < sizeof(bss) / sizeof(bss[0])) {
          int val = inputStr.substring(2, 5).toInt();
          val = constrain(val, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
          bss[servoId].write(val);                  // sets the servo position according to the scaled value
        }
      }
    }
  }
}

void setup() {
  bss[0].registerPin(-1, 3);
  bss[1].registerPin(-1, 5);
  bss[2].registerPin(-1, 6);
  bss[3].registerPin(-1, 9);
  Serial.begin(9600);

  pinMode(2, INPUT_PULLUP); //global button

  //  //optinal mode selection hardware
  //  pinMode(6, INPUT_PULLUP);
  //  pinMode(7, INPUT_PULLUP);
  //  pinMode(8, INPUT_PULLUP);
}

int pastButtonState = HIGH;
int state = 0;

void loop() {
  // choose a way of control from Push, Click, or Serial.
  //    for (int i = 0; i < sizeof(bss) / sizeof(bss[0]); i++) bss[i].rotateByPush();
  //  for (int i = 0; i < sizeof(bss) / sizeof(bss[0]); i++) bss[i].rotateByClick();
  rotateBySerial();

  int currentButtonState = digitalRead(2);
  if (pastButtonState == HIGH && currentButtonState == LOW) {
    state = 1 - state;
    int val = (state == 0) ? 0 : 180;
    for (int i = 0; i < sizeof(bss) / sizeof(bss[0]); i++) {
      bss[i].write(val);                  // sets the servo position according to the scaled value}
    }
  }
  pastButtonState = currentButtonState;

  //  //optinal mode selection hardware
  //  if (digitalRead(6) == LOW) for (int i = 0; i < sizeof(bss) / sizeof(bss[0]); i++) bss[i].rotateByPush();
  //  else if (digitalRead(7) == LOW) for (int i = 0; i < sizeof(bss) / sizeof(bss[0]); i++) bss[i].rotateByClick();
  //  else if (digitalRead(8) == LOW) rotateBySerial();
}

