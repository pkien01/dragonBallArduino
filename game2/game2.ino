#include <LiquidCrystal.h>
#include <Wire.h>
#include <assert.h>

template<class T, size_t maxSize> 
class Container {
  T arr[maxSize];
  int sz;
public:
  Container() {
    sz = 0;
  }
  int add(const T& elem) {
    if (sz >= maxSize) {
      assert(false);
    } else {
      arr[sz++] = elem;
    }
  }
  T& operator[](int i) {
    return arr[i];
  }
  T& back() {
    return arr[sz - 1];
  }
  int size() {
    return sz;
  }
};

class Canvas {
  byte rows[8];
public:
  Canvas() {
    memset(rows, 0, sizeof(byte) * 8);
  }
  Canvas(byte _rows[8]) {
    memcpy(rows, _rows, 8);
  }
  Canvas(const Canvas &other) {
    memcpy(rows, other.rows, 8);
  }
  bool empty() {
    for (int i = 0; i < 8; i++) {
      if (rows[i]) {
        return false;
      }
    }
    return true;
  }
  void print() {
    for (int i = 0; i < 8; i++) {
      for (int j = 4; j >= 0; j--) Serial.print(rows[i] >> j & 1);
      Serial.println();
    }
  }
  void createChar(LiquidCrystal &lcd, int idx) {
    lcd.createChar(idx, rows);
  }
  bool operator==(const Canvas &other) const {
    for (int i = 0; i < 8; i++) {
      if (this->rows[i] != other.rows[i]) {
        return false;
      }
    }
    return true;
  }
  bool operator!=(const Canvas &other) const {
    return !(*this == other);
  }
  Canvas merge(const Canvas &other) {
    for (int i = 0; i < 8; i++) {
      this->rows[i] |= other.rows[i];
    }
    return *this;
  } 
  Canvas reverseRows() {
    Canvas res;
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 5; j++) {
        res.rows[i] |= (this->rows[i] >> j & 1) << (4 - j);
      }
    }
    return res;
  }
};

class Entity {
protected:    
  int r, c;
  int statePtr;
  Container<Canvas, 10> states;
  bool hide;
  char label;
public:
  Entity(int _row, int _col, int _statePtr) {
    r = _row;
    c = _col;
    statePtr = _statePtr;
    hide = false;
    label = 0;
  }
  char getLabel() {
    return label;
  }
  void setState(int _statePtr) {
    statePtr = _statePtr;
  }
  Canvas getState() {
    return states[statePtr];
  }
  int row() const { return r; }
  int col() const { return c; }
  bool hidden() const {return hide; }
};

class Frame {
  Container<Entity*, 5> entities;
  Container<Canvas, 8> customChars;
  Canvas screen[2][16];
public: 
  void addEntity(Entity *entity) {
    entities.add(entity);
  }
  void draw(LiquidCrystal &lcd) {
    Canvas newScreen[2][16];
    Container<Canvas, 8> requiredChars;
    for (int i = 0; i < entities.size(); i++) {
      // Serial.print("Entity ");
      // Serial.println(i);
      // Serial.print("row: ");
      // Serial.println(entities[i]->row());
      // Serial.print("col: ");
      // Serial.println(entities[i]->col());
      // Serial.println("***************");
      // Serial.flush();
      if (!entities[i]->hidden()) {
        requiredChars.add(newScreen[entities[i]->row()][entities[i]->col()].merge(entities[i]->getState()));
      }
    }

    int used = 0, remain = 0;
    for (int i = 0; i < requiredChars.size(); i++) {
      bool has = false;
      for (int j = 0; j < customChars.size(); j++) {
        if (customChars[j] == requiredChars[i]) {
          used |= (1 << j);
          has = true;
          break;
        }
      }
      if (has) remain |= (1 << i);
    }

    for (int i = 0, j = 0; i < requiredChars.size(); i++) {
      if (!(remain >> i & 1)) {
        while (j < customChars.size() && (used >> j & 1)) {
          j++;
        }
        if (j >= customChars.size()) {
          customChars.add(requiredChars[i]);
        } else {
          customChars[j++] = requiredChars[i];
        }
      }
    }

    // Serial.print("Drawing ");
    // Serial.print(customChars.size());
    //  Serial.println(" entities:");
    // for (int i = 0; i < customChars.size(); i++) {
    //   customChars[i].print();
    //   Serial.println();
    // }
    // Serial.flush();

    for (int i = 0; i < customChars.size(); i++) {
      if (!(used >> i & 1)) customChars[i].createChar(lcd, i);
    }

    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 16; j++) {
        if (newScreen[i][j].empty()) {
          lcd.setCursor(j, i);
          lcd.print(" ");
        } else if (newScreen[i][j] != screen[i][j]) {
          lcd.setCursor(j, i);
          int id = -1;
          for (int k = 0; k < customChars.size(); k++) {
            if (customChars[k] == newScreen[i][j]) {
              id = k;
              break;
            }
          }
          // Serial.print("i: ");
          // Serial.println(i);
          // Serial.print("j: ");
          // Serial.println(j);
          // Serial.print("k: ");
          // Serial.println(id);
          // Serial.flush();

          lcd.write(byte(id));
        }
      }
    }

    for (int i = 0; i < entities.size(); i++) {
      // Serial.print("Entity ");
      // Serial.println(i);
      // Serial.print("row: ");
      // Serial.println(entities[i]->row());
      // Serial.print("col: ");
      // Serial.println(entities[i]->col());
      // Serial.println("***************");
      // Serial.flush();
      if (!entities[i]->hidden() && entities[i]->getLabel() > 0) {
        lcd.setCursor(entities[i]->col(), entities[i]->row() == 0? 1 : 0);
        lcd.print(entities[i]->getLabel());
      }
    }
    delay(200);
  }
};

constexpr byte blastTravel[7][8] = {
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
  },
  {
    B00000,
    B00000,
    B00000,
    B00001,
    B00001,
    B00000,
    B00000,
    B00000
  }
};
class Projectile:  public Entity {
  int counter;
  Entity *entity;
  bool stop;
  int dir;
public:
  Projectile(Entity *_entity, int _dir): entity(_entity), Entity(_entity->row(), _entity->col() + _dir, 0)  {
    for (int i = 0; i < 6; i++) {
      states.add(Canvas(blastTravel[i]));
    }
    setState(_dir == 1? 0 : 5);
    hide = true;
    stop = true;
    dir = _dir;
  }
  void update() {
    if (stop) {
      Serial.print(r);
      Serial.print(" ");
      Serial.println(c);
      Serial.print("counter: ");
      Serial.println(counter);
      Serial.flush();
    }
    if (hide) return;
    if (counter < 0 || counter >= 6) {
      counter = dir == 1? 0 : 6;
      c += dir;
      if (c < 0 || c > 15) {
        if (stop) hide = true;
        else c = entity->col() + dir;
      }
    }
    setState(counter);
    counter += dir == 1? 4 : -4;
  }
  void setStop() {
    stop = true;
  }
  void unsetStop() {
    stop = false;
    if (hide) {
      hide = false;
      r = entity->row();
      c = entity->col() + dir;
      counter = 0;
    }
  }
};

constexpr byte runningBytes[8] = {
  B01100,
  B01100,
  B00000,
  B01111,
  B11100,
  B01100,
  B11010,
  B10011
};

constexpr byte standingBytes[8] = {
  B01100,
  B01100,
  B00000,
  B01100,
  B01100,
  B01100,
  B01100,
  B01110
};

constexpr byte jumping1Bytes[8] = {
  B01100,
  B01100,
  B00000,
  B01100,
  B01100,
  B01110,
  B00110,
  B00111
};

constexpr byte jumping2Bytes[8] = {
  B01100,
  B01100,
  B00000,
  B01100,
  B01111,
  B01111,
  B00011,
  B00000
};

constexpr byte shootingBytes[8] = {
  B01100,
  B01100,
  B00000,
  B01111,
  B01111,
  B01100,
  B11010,
  B10011
};


class Human: public Entity {
  int runningCounter;
  int jumpingCounter;
  Projectile blast;
public:
  Human(int initR, int initC): Entity(initR, initC, 0), blast(this, 1) {
    states.add(Canvas(standingBytes));
    states.add(Canvas(runningBytes));
    states.add(Canvas(jumping1Bytes));
    states.add(Canvas(jumping2Bytes));
    states.add(Canvas(shootingBytes));
    runningCounter = 0;
    jumpingCounter = 0;
    setState(0);
  }
  void runRight() {
    if (statePtr == 0) {
      setState(1);
    } else {
      setState(0);
    }
    if (runningCounter == 16) {
      runningCounter = 0;
      if (c < 15) {
        c++;
      }
    }
    runningCounter++;
  }
  void runLeft() {
    if (statePtr == 0) {
      setState(1);
    } else {
      setState(0);
    }
    if (runningCounter == 16) {
      runningCounter = 0;
      if (c > 0) {
        c--;
      }
    }
    runningCounter++;
  }
  void jump() {
    if (statePtr == 0) {
      setState(2);
    } else if (statePtr == 2) {
      setState(3);
      r = 1;
    } else if (statePtr ==3) {
      setState(2);
      r = 0;
    } else {
      setState(0);
    }
  }
  void resetFromJump() {
    r = 1;
    setState(0);
  }
  void shoot() {
    setState(4);
    blast.unsetStop();
  }
  Projectile& getBlast() {
    return blast;
  }
};

class Android: public Entity {
  int runningCounter;
  int jumpingCounter;
  Projectile blast;
public:
  Android(int initR, int initC): Entity(initR, initC, 0), blast(this, -1) {
    states.add(Canvas(standingBytes).reverseRows());
    states.add(Canvas(runningBytes).reverseRows());
    states.add(Canvas(jumping1Bytes).reverseRows());
    states.add(Canvas(jumping2Bytes).reverseRows());
    states.add(Canvas(shootingBytes).reverseRows());
    runningCounter = 0;
    jumpingCounter = 0;
    setState(0);
  }
  void runRight() {
    if (statePtr == 0) {
      setState(1);
    } else {
      setState(0);
    }
    if (runningCounter == 16) {
      runningCounter = 0;
      if (c < 15) {
        c++;
      }
    }
    runningCounter++;
  }
  void runLeft() {
    if (statePtr == 0) {
      setState(1);
    } else {
      setState(0);
    }
    if (runningCounter == 16) {
      runningCounter = 0;
      if (c > 0) {
        c--;
      }
    }
    runningCounter++;
  }
  void jump() {
    if (statePtr == 0) {
      setState(2);
    } else if (statePtr == 2) {
      setState(3);
      r = 1;
    } else if (statePtr ==3) {
      setState(2);
      r = 0;
    } else {
      setState(0);
    }
  }
  void resetFromJump() {
    r = 1;
    setState(0);
  }
  void shoot() {
    setState(4);
    blast.unsetStop();
  }
  Projectile& getBlast() {
    return blast;
  }
  void update() {
    shoot();
  }
};


constexpr int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Frame frame;
Human player(1, 0);
Android opponent(1, 15);
void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  frame.addEntity(&player);
  frame.addEntity(&player.getBlast());
  frame.addEntity(&opponent);
  frame.addEntity(&opponent.getBlast());
  Wire.begin(9); 
  Wire.onReceive(receiveEvent);
}

char prevCommand = 0;
void receiveEvent(int bytes) {
//  Serial.print("received: ");
  char command = Wire.read();
  // Serial.println(command);
  // Serial.flush();
  player.getBlast().setStop();
  if (prevCommand == 'j' && command != 'j') {
     player.resetFromJump();
  }
  switch (command) {
    case 'l': player.runLeft(); break;
    case 'r': player.runRight(); break;
    case 'b': player.shoot(); break;
    case 'j': player.jump(); break;
  }
  prevCommand = command;
}

void loop() {
  // put your main code here, to run repeatedly:
  frame.draw(lcd);
  player.getBlast().update();
  opponent.update();
  opponent.getBlast().update();
}
