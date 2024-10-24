#include <Wire.h>

constexpr int deviation = 10;

struct Button {
  int threshold;
} leftButton, blastButton, jumpButton, rightButton;


int buttonValue;
inline bool isPressed(Button button) {
  return abs(buttonValue - button.threshold) < deviation;
}

void setup() {
  Serial.begin(9600);

  leftButton.threshold = 332; 
  blastButton.threshold = 443; 
  jumpButton.threshold = 499; 
  rightButton.threshold = 532;

  Wire.begin(); 
}

void loop() {
  buttonValue = analogRead(A0);
  char buttonPressed;
  if (isPressed(jumpButton)) {
    buttonPressed = 'j';
  } else if (isPressed(blastButton)) {
    buttonPressed = 'b';
  } else if (isPressed(leftButton)) {
    buttonPressed = 'l';
  } else if (isPressed(rightButton)) {
    buttonPressed = 'r';
  } else {
    buttonPressed = 'n';
  }
  if (buttonPressed != 'n') {
    Serial.print("buttonPressed: ");
    Serial.println(buttonPressed);
  }
  Wire.beginTransmission(9); // transmit to device #9
  Wire.write(char(buttonPressed));              // sends x 
  Wire.endTransmission();    // stop transmitting
}
