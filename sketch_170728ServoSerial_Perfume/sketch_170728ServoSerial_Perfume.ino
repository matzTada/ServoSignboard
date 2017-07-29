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
      pinMode(buttonPin, INPUT_PULLUP);
      attach(servoPin);
    }
    boolean isButtonPushed() {
      if (digitalRead(buttonPin) == LOW) return true;
      else return false;
    }
    boolean isButtonClicked() {
      int currentButtonState = digitalRead(buttonPin);
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

ButtonServo bss[3];

void rotateBySerial() { // example command : S1180 means that the servo whose id = 1 rotates 180 degrees
  if (Serial1.available() > 0) {
    String inputStr = Serial1.readStringUntil("\n");
    if (inputStr.charAt(0) == 'S') {
      int servoId = (int)(inputStr.charAt(1) - '0');
      if (0 <= servoId && servoId < sizeof(bss) / sizeof(bss[0])) {
        int val = inputStr.substring(2, 5).toInt();
        val = constrain(val, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
        bss[servoId].write(val);                  // sets the servo position according to the scaled value
      }
    }
  }
}

void setup() {
  bss[0].registerPin(2, 3);
  bss[1].registerPin(4, 5);
  bss[2].registerPin(10, 11);
  Serial1.begin(9600);

  //optinal mode selection hardware
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
}

void loop() {
  // choose a way of control from Push, Click, or Serial.
  //  for (int i = 0; i < sizeof(bss) / sizeof(bss[0]); i++) bss[i].rotateByPush();
  //  for (int i = 0; i < sizeof(bss) / sizeof(bss[0]); i++) bss[i].rotateByClick();
  //  rotateBySerial();

  //optinal mode selection hardware
  if (digitalRead(6) == LOW) for (int i = 0; i < sizeof(bss) / sizeof(bss[0]); i++) bss[i].rotateByPush();
  else if (digitalRead(7) == LOW) for (int i = 0; i < sizeof(bss) / sizeof(bss[0]); i++) bss[i].rotateByClick();
  else if (digitalRead(8) == LOW) rotateBySerial();
}

