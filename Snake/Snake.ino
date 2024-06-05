#include <charMap.h>
#include <GyverOLED.h>
#define pinX    A2  // ось X джойстика
#define pinY    A1  // ось Y джойстика
#define swPin    9  // кнопка джойстика
#define ledPin  13  // светодиод на Pin 13
GyverOLED<SSD1306_128x32, OLED_NO_BUFFER> oled;

class snake {
  private:
  static const short int gameWidth = 32;
  static const short int gameHeight = 8;
  static const short int screenK = 32/gameHeight;
  
  public:
  byte bitMap[gameWidth][gameHeight], oldBitMap[gameWidth][gameHeight];
  short int headX, headY, rotation;
  short int score;
  short int appleX, appleY;

  short int tailCords[gameWidth * gameHeight], tailLength;

  void ResetValues(){
    headX = 0;
    headY = 0;
    ClearBitMap();
    ClearTailCords();
    tailLength = 1; // один пиксель позади головы
    rotation = 1; // 0 вверх, 1 вправо, 2 вниз, 3 влево
    score = 0;
    RandomApple();
  }

  void ClearBitMap(){
    for(short int w = 0; w < gameWidth; w++){
      for(short int h = 0; h < gameHeight; h++){
        bitMap[w][h] = 0;
      }
    }
  }
  void AddTailPiece(short int x,short int y){
    tailLength++;
    tailCords[tailLength] = (y * gameWidth) + x;
  }
  void ClearTailCords(){
    for(short int i = 0; i < gameWidth * gameHeight; i++){
      tailCords[i] = 0;
    }
  }
  void DrawHead(){
    oled.rect(headX*screenK, headY*screenK, (headX*screenK)+screenK-1, (headY*screenK)+screenK-1, 1);
  }
  void Step(){
    if(headX < 0)
      headX = gameWidth-1;
    if(headX > gameWidth-1)
      headX = 0;
    if(headY < 0)
      headY = gameHeight-1;
    if(headY > gameHeight-1)
      headY = 0;
      AddTailPiece(headX, headY);
    if(tailLength != score)updateTails();      
  }
  void joySnake(){
        if(rotation == 0)
      headY--;
    if(rotation == 1)
      headX++;
    if(rotation == 2)
      headY++;
    if(rotation == 3)
      headX--;
      
  }
  void updateTails(){
    ClearTailPeace(0);
    for(short int i=0; i < tailLength+1; i++){
      tailCords[i] = tailCords[i+1];
    }
  }
  void ClearTailPeace(short int num){
        short int tailY = floor(tailCords[num] / gameWidth);
        short int tailX = tailCords[num] - (tailY * gameWidth);
        oled.rect(tailX*screenK, tailY*screenK, (tailX*screenK)+screenK, (tailY*screenK)+screenK-1, 0);
        tailCords[num] = 0;
        tailLength--;
     }

     void RandomApple(){
      byte coincidences = 0;
      short int rndX = random(0, gameWidth-1);
      short int rndY = random(0, gameHeight-1);
      if(rndX < 3 and rndY == 0) rndY++;
      appleX = rndX;
      appleY = rndY;
     }
     void DrawApple(){
      oled.circle(appleX*screenK+(screenK/2), appleY*screenK+(screenK/2), (screenK/2), 1);
     }
     
};

snake game;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(swPin, INPUT);
  digitalWrite(swPin, HIGH);
  oled.init();
  oled.clear();
  oled.setScale(1);
  game.ResetValues();
 
}



void loop() {
  joyInit();
  game.DrawApple();
  game.joySnake(); 
  game.DrawHead();
  game.Step();
  
  oled.setCursor(0,0);
  oled.print("Счёт: ");
  oled.setCursor(32,0);
  oled.print(game.score);
  delay(150);

  if(game.headX == game.appleX and game.headY == game.appleY){
    game.RandomApple();
    game.score++;
    game.tailLength++;
  }
}

void joyInit(){
  boolean ledState = digitalRead(swPin); // считываем состояние кнопки
  digitalWrite(ledPin, ledState);        // вкл./выкл. светодиод

  short int joyX = analogRead(pinX);    // считываем значение оси Х
  short int joyY = analogRead(pinY);    // считываем значение оси Y

  joyX = map(joyX, 0, 1023, 0, 255); // преобразуем значение X в другой диапазон
  joyY = map(joyY, 0, 1023, 0, 255); // преобразуем значение Y в другой диапазонs

  const short int freezeZone = 64;
  if(joyX < 128 - freezeZone)
    game.rotation = 1;
   if(joyX > 128 + freezeZone)
    game.rotation = 3;
   if(joyY < 128 - freezeZone)
    game.rotation = 0;
   if(joyY > 128 + freezeZone)
    game.rotation = 2;
}

