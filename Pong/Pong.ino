#include <charMap.h>
#include <GyverOLED.h>
#define pinX    A2  // ось X джойстика
#define pinY    A1  // ось Y джойстика
#define swPin    9  // кнопка джойстика
#define ledPin  13  // светодиод на Pin 13
GyverOLED<SSD1306_128x32, OLED_NO_BUFFER> oled;

class pingpong {
  private:
    static const short int gameWidth = 32;
    static const short int gameHeight = 8;
    static const short int screenK = 32 / gameHeight;

  public:
    short int paddlePlayerY = 1;
    short int paddleBotY = 1;

    short int ballX = centerW, ballY = centerH;
    short int ballDirectionX = 1, ballDirectionY = 1;

    short int scorePlayer = 0;
    short int scoreBot = 0;

    short int centerW = (gameWidth / 2) - 1;
    short int centerH = (gameHeight / 2) - 1;

    short int timerBall = 0, timerBot = 0;

    void BallReset(){
      ballX = centerW + random(-1,1);
      ballY = centerH + random(-1,1);
      ballDirectionX = 1;
      ballDirectionY = 1;
      paddlePlayerY = centerH;
      paddleBotY = centerH;
    }
    void BotThinks() {
      timerBot++;
      if(timerBot<24){
        if(ballX > centerW-2){
          if(ballY > paddleBotY and ballY > paddleBotY-1) paddleBotY++;
          if(ballY < paddleBotY and ballY < paddleBotY-1) paddleBotY--;
        }
      }
      if(timerBot>25) timerBot = 0;
    }
    void BallStep() {
      timerBall++;
      if(timerBall>1){
      timerBall = 0;
      
      oled.clear(ballX * screenK - 1, ballY * screenK, ballX * screenK + screenK, ballY * screenK + (screenK - 1));

      ballX += ballDirectionX;
      ballY += ballDirectionY;
      if (ballY < 1)ballDirectionY = -1 * ballDirectionY;
      if (ballY > gameHeight - 2)ballDirectionY = -1 * ballDirectionY;
      // паллет игрока
      if (ballX < 2){
        if(ballY == paddlePlayerY or ballY == paddlePlayerY+1){
          ballDirectionX = -1 * ballDirectionX;
        }else{
          if(ballX < 0){
            scoreBot++;
            delay(1000);
            BallReset();
          }}}
      // паллет бота
      if (ballX > gameWidth - 3){
        if(ballY == paddleBotY or ballY == paddleBotY+1){
          ballDirectionX = -1 * ballDirectionX;
        }else{
          if(ballX > gameWidth - 1){
            scorePlayer++;
            delay(1000);
            BallReset();
          }}}

      oled.rect(ballX * screenK, ballY * screenK, ballX * screenK + (screenK - 1), ballY * screenK + (screenK - 1), 1);
      }}

      void ClearOldPaddles() {
        oled.clear(0, paddlePlayerY * screenK, screenK, (paddlePlayerY + 2)*screenK - 1);
        oled.clear((gameWidth - 1)*screenK, (paddleBotY * screenK), ((gameWidth - 0) * screenK), (paddleBotY + 2)*screenK - 1);
        //oled.clear((gameWidth - 1)*screenK, (paddleBotY * screenK), (gameWidth * screenK) + 1, (paddleBotY + 2)*screenK - 1);
      }
      void DrawPaddles() {
        if (paddlePlayerY < 0)
          paddlePlayerY = 0;
        if (paddlePlayerY > gameHeight - 2)
          paddlePlayerY = gameHeight - 2;
        if (paddleBotY < 0)
          paddleBotY = 0;
        if (paddleBotY > gameHeight - 2)
          paddleBotY = gameHeight - 2;

        oled.rect(0, paddlePlayerY * screenK, screenK - 1, (paddlePlayerY + 2)*screenK - 1, 1);
        oled.rect((gameWidth - 1)*screenK, (paddleBotY * screenK), ((gameWidth - 0) * screenK), (paddleBotY + 2)*screenK - 1, 1);
      }
    };

    pingpong game;

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
      game.BallReset();
    }



    void loop() {
      game.ClearOldPaddles();
      joyInit();
      game.BallStep();
      game.BotThinks();
      game.DrawPaddles();
      oled.setCursor(48, 0);
      oled.print(game.scorePlayer);
      oled.setCursor(80, 0);
      oled.print(game.scoreBot);
      delay(33);
    }

    void joyInit() {
      boolean ledState = digitalRead(swPin); // считываем состояние кнопки
      digitalWrite(ledPin, ledState);        // вкл./выкл. светодиод

      short int joyX = analogRead(pinX);    // считываем значение оси Х
      short int joyY = analogRead(pinY);    // считываем значение оси Y

      joyX = map(joyX, 0, 1023, 0, 255); // преобразуем значение X в другой диапазон
      joyY = map(joyY, 0, 1023, 0, 255); // преобразуем значение Y в другой диапазонs

      const short int freezeZone = 64;
      if (joyY < 128 - freezeZone)
        game.paddlePlayerY--;
      if (joyY > 128 + freezeZone)
        game.paddlePlayerY++;
    }

