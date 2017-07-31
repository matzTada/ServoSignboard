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
      attach(servoPin, 800, 1500); // parameters for GWS 2BBMG ref: http://tetsuakibaba.jp/index.php?page=workshop/ServoBasis/main
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
int servoDelay = 500;

void rotateBySerial() { // example command : S1180 means that the servo whose id = 1 rotates 180 degrees
  if (Serial.available() > 0) {
    String inputStr = Serial.readStringUntil("\n");
    if (inputStr.charAt(0) == 'S') {
      char c1 = inputStr.charAt(1);
      if (c1 == 'A') {
        int val = inputStr.substring(2, 5).toInt();
        val = constrain(val, 0, 179);     // scale it to use it with the servo (value between 0 and 180)
        for (int i = 0; i < sizeof(bss) / sizeof(bss[0]); i++) {
          bss[i].attach(bss[i].servoPin, 800, 1500);
          bss[i].write(val);                  // sets the servo position according to the scaled value}
          delay(servoDelay);
          bss[i].detach();
        }
      } else if (c1 == 'D') {
        int val = inputStr.substring(2, 5).toInt();
        servoDelay = val * 10;     // scale it to use it with the servo (value between 0 and 180)
        Serial.println(servoDelay);
      } else {
        int i = (int)(inputStr.charAt(1) - '0');
        if (0 <= i && i < sizeof(bss) / sizeof(bss[0])) {
          int val = inputStr.substring(2, 5).toInt();
          val = constrain(val, 0, 179);     // scale it to use it with the servo (value between 0 and 180)
          bss[i].attach(bss[i].servoPin, 800, 1500);
          bss[i].write(val);                  // sets the servo position according to the scaled value}
          delay(servoDelay);
          bss[i].detach();
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
}

int pastButtonState = HIGH;
int state = 0;

void loop() {
  // choose a way of control from Push, Click, or Serial.
  rotateBySerial();

  int currentButtonState = digitalRead(2);
  if (pastButtonState == HIGH && currentButtonState == LOW) {
    state = 1 - state;
    int val = (state == 0) ? 0 : 179;
    for (int i = 0; i < sizeof(bss) / sizeof(bss[0]); i++) {
      bss[i].attach(bss[i].servoPin, 800, 1500);
      bss[i].write(val);                  // sets the servo position according to the scaled value}
      delay(servoDelay);
      bss[i].detach();
    }
  }
  pastButtonState = currentButtonState;
}

