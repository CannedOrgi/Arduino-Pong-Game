#include <Arduino.h>

const int leftButtonPin = 7;  // Button to move the paddle left
const int rightButtonPin = 8; // Button to move the paddle right
const int buzzerPin = 9;

void setup() {
  Serial.begin(9600);
  pinMode(leftButtonPin, INPUT);
  pinMode(rightButtonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  static bool prevLeftButtonState = HIGH;
  static bool prevRightButtonState = HIGH;

  bool leftButtonState = digitalRead(leftButtonPin);
  bool rightButtonState = digitalRead(rightButtonPin);

  //Send 'L' if the left button is pressed
  if (leftButtonState == LOW && prevLeftButtonState == HIGH) {
    Serial.write('L');
  }

  //Send 'R' if the right button is pressed
  if (rightButtonState == LOW && prevRightButtonState == HIGH) {
    Serial.write('R');
  }

  prevLeftButtonState = leftButtonState;
  prevRightButtonState = rightButtonState;
  if (Serial3.available() > 0) {
    char command = Serial3.read();
    if (command == 'BUZZ'){
      tone(buzzerPin, 1000, 500);
      delay(1000);
      noTone(buzzerPin);
    }
  }


  delay(10);
}