// include the library code:
#include <LiquidCrystal.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
constexpr int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

constexpr byte buttonPin = 6;
constexpr unsigned long debounceTime = 200;
unsigned long buttonPressedPreviousTime = 0;

int cnt = 0;

void setup() {
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");

  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:

  unsigned long currentTime = millis();

  bool isButtonPressed = digitalRead(buttonPin) == LOW;
  Serial.println(isButtonPressed);
  if (isButtonPressed && (currentTime - buttonPressedPreviousTime) > debounceTime) {
    buttonPressedPreviousTime = currentTime;
    cnt++;
  }
  lcd.print(cnt);

}