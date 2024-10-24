#include <LiquidCrystal.h>
#include <Wire.h>

constexpr int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
constexpr byte buttonPin = 6;
constexpr unsigned long debounceTime = 150;
unsigned long buttonPressedPreviousTime = 0;

constexpr byte shooting[] = {
  B01100,
  B01100,
  B00000,
  B01111,
  B01111,
  B01100,
  B11010,
  B10011
};

constexpr byte blastTravel[5][8] = {
  {
    B00000,
    B00000,
    B00000,
    B10000,
    B10000,
    B00000,
    B00000,
    B00000
  },
  {
    B00000,
    B00000,
    B00000,
    B11000,
    B11000,
    B00000,
    B00000,
    B00000
  },
  {
    B00000,
    B00000,
    B00000,
    B01100,
    B01100,
    B00000,
    B00000,
    B00000
  },
  {
    B00000,
    B00000,
    B00000,
    B00110,
    B00110,
    B00000,
    B00000,
    B00000
  },
  {
    B00000,
    B00000,
    B00000,
    B00011,
    B00011,
    B00000,
    B00000,
    B00000
  }
};

constexpr byte running[] = {
  B01100,
  B01100,
  B00000,
  B01111,
  B11100,
  B01100,
  B11010,
  B10011
};

byte standing[] = {
  B01100,
  B01100,
  B00000,
  B01100,
  B01100,
  B01100,
  B01100,
  B01110
};

byte jumping1[] = {
  B01100,
  B01100,
  B00000,
  B01100,
  B01100,
  B01110,
  B00110,
  B00111
};

byte jumping2[] = {
  B01100,
  B01100,
  B00000,
  B01100,
  B01111,
  B01111,
  B00011,
  B00000
};


struct Entity {
  int pos = 0, direction = 0;
};

Entity person;
Entity blast;

bool isBlasting = false;
bool isJumping = false;

void setup() {
  pinMode(A0, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  // lcd.createChar(0, shooting);
  // lcd.createChar(1, running);
  // lcd.createChar(2, standing);
  // lcd.createChar(3, jumping1);
  // lcd.createChar(4, jumping2);

  // for (int i = 0; i < 5; i++) {
  //   lcd.createChar(5 + i, blastTravel[i]);
  // }

  Wire.begin(9); 
  Wire.onReceive(receiveEvent);
}

void receiveEvent(int bytes) {
  Serial.print("received: ");
  char command = Wire.read();
  Serial.println(command);
  isBlasting = false;
  isJumping = false;
  person.direction = 0;
  switch (command) {
    case 'l': person.direction = -1; break;
    case 'r': person.direction = 1; break;
    case 'b': isBlasting = true; break;
    case 'j': isJumping = true; break;
  }
}

void displayBackground() {
  lcd.setCursor(8, 0);
  lcd.print("Score: 0");
}

void animateJumping() {
  lcd.createChar(0, standing);
  lcd.createChar(1, jumping1);
  lcd.createChar(2, jumping2);

  int screenPos = person.pos >> 1;

  lcd.setCursor(screenPos, 1);
  lcd.write(byte(1)); 
  delay(100);
  
  lcd.setCursor(screenPos, 0);
  lcd.write(byte(2)); 
  lcd.setCursor(screenPos, 1);
  lcd.print(" "); 
  delay(200);

  lcd.setCursor(screenPos, 1);
  lcd.write(byte(1));
  lcd.setCursor(screenPos, 0);
  lcd.print(" "); 
  delay(100);

  lcd.setCursor(screenPos, 1);
  lcd.write(byte(0)); 
  
  delay(100);
}

void animateRunning() {
  lcd.createChar(0, running);
  lcd.createChar(1, standing);

  lcd.clear(); 
  displayBackground();

  int screenPos = person.pos >> 1;

  for (byte i = 0; i <= 1; i++) {
    lcd.setCursor(screenPos, 1);
    lcd.write(i); 
    delay(200);
  }

  if (person.pos + person.direction >= 0 && screenPos + person.direction <= 15) {
    person.pos += person.direction;
  }
}

void animateStanding() {
  lcd.createChar(0, standing);
  //lcd.clear(); 
  displayBackground();
  lcd.setCursor(person.pos >> 1, 1);
  lcd.write(byte(0));
  delay(200);
}

void animateShooting() {
   lcd.createChar(0, shooting);
  for (int i = 0; i < 5; i++) {
    lcd.createChar(1 + i, blastTravel[i]);
  }

  int screenPos = blast.pos / 5;
  int personScreenPos = person.pos >> 1;
  
  if (screenPos <= personScreenPos || screenPos > 15) {
    screenPos = personScreenPos + 1;
    blast.pos = screenPos * 5;
    if (!isBlasting) return;
  } 
  blast.direction = 5;
  
  for (byte i = 0; i < 5; i++) { 
    lcd.clear(); 
    displayBackground();
    lcd.setCursor(personScreenPos, 1);
    lcd.write(byte(0));
    lcd.setCursor(screenPos, 1);
    lcd.write(1 + i); 
    delay(35);
  }
  blast.pos += blast.direction;
}

bool isRunning = true;
int cnt = 0;
void loop() {
  if (isBlasting) {
    animateShooting();
  } else if (isJumping) {
    animateJumping();
  } else if (person.direction != 0) {
    animateRunning();
  } else {
    animateStanding();
  }
}
