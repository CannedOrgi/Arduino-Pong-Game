#include <Adafruit_GFX.h>
#include <EEPROM.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>


#define Black 0x0000
#define White 0xFFFF
#define Red 0xF800
#define Green 0x07E0
#define Yellow 0xFFE0
#define Orange 0xFD20
#define Cyan 0x07FF

const int XP = 8, XM = A2, YP = A3, YM = 9;

bool isBreakoutSelected = false;
bool isPongSelected = false;
int pixelX, pixelY;

MCUFRIEND_kbv display;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_GFX_Button leftButton, rightButton, startButton, breakoutButton, pongButton, exitButton;

void setup() {

  auto displayId = display.readID();
  display.begin(displayId);
  display.fillScreen(Black);
  display.setRotation(0);

  leftButton.initButton(&display, 42, 292, 77, 50, 0x0000, 0x0000, 0xFFFF, "<-", 3);
  rightButton.initButton(&display, 202, 292, 77, 50, 0x0000, 0x0000, 0xFFFF, "->", 3);

  startButton.initButton(&display, 120, 150, 150, 52, 0x0000, 0x0000, 0x0000, "", 3);
  exitButton.initButton(&display, 20, 10, 40, 22, 0x0000, 0x0000, 0xFFFF, "x", 3);

  breakoutButton.initButton(&display, 120, 60, 150, 52, 0x0000, 0x0000, 0x0000, "", 3);
  pongButton.initButton(&display, 120, 100, 100, 52, 0x0000, 0x0000, 0x0000, "", 3);

  Serial3.begin(9600);
  Serial.begin(9600);

}
class Utility {
public:
  void showHomeScreen() {
    display.setTextSize(2);
    display.setCursor(40, 10);
    display.setTextColor(White);
    display.print("Choose a Game");

    display.setTextSize(3);
    display.setCursor(30, 60);
    display.setTextColor(Red);
    display.print("1. BREAKOUT");
    display.setTextSize(3);
    display.setCursor(30, 100);
    display.setTextColor(Orange);
    display.print("2. PONG");

    display.setTextSize(2);
    display.setCursor(10, 160);
    display.setTextColor(Green);
    display.print("Design Lab Project");

    display.setTextSize(1);
    display.setCursor(30, 195);
    display.setTextColor(Cyan);
    display.print("By:-");
    display.setTextSize(1);
    display.setCursor(30, 210);
    display.setTextColor(Cyan);
    display.print("Rahul Vivek");
    display.setTextSize(1);
    display.setCursor(30, 220);
    display.setTextColor(Cyan);
    display.print("Priyanshu");

    display.setTextSize(1);
    display.setCursor(30, 240);
    display.setTextColor(Cyan);
    display.print("Under:- Prof. Ribhu Chopra");

    drawBreakoutPattern(true);
  }
  void drawBreakoutPattern(bool isBottom = false) {
    int colors[] = { Red, Orange, Yellow, Green };
    for (int i = 0; i < 4; ++i) {
      int calci = (i + 3) * 10 + i;
      int currY = isBottom ? 330 - calci : calci;
      for (int j = 0; j < 4; j++) {
        int currX = 60 * j;
        display.fillRoundRect(currX, currY, 59, 10, 3, colors[i]);
      }
    }
  }
  bool isScreenTouched() {
    TSPoint touchPoint = ts.getPoint();
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);
    digitalWrite(XM, HIGH);
    bool isPressed = (touchPoint.z > ts.pressureThreshhold);
    if (isPressed) {
      // maps val,fromLow,fromHigh,toLow,toHigh
      pixelX = map(touchPoint.x, 122, 942, 0, 240);
      pixelY = map(touchPoint.y,111, 890, 0, 320);
    }
    return isPressed;
  }
  void monitorHomeScreenClicks() {
    if (isScreenTouched()) {
      breakoutButton.press(breakoutButton.contains(pixelX, pixelY));
      pongButton.press(pongButton.contains(pixelX, pixelY));
      if (breakoutButton.justPressed()) {
        display.fillScreen(Black);
        isBreakoutSelected = true;
      } else if (pongButton.justPressed()) {
        display.fillScreen(Black);
        isPongSelected = true;
      }
    }
  }
};

Utility utility;

class Game {
protected:
  int paddlePosition;
  int ballPositionX;
  int ballPositionY;
  int ballSpeedX;
  int ballSpeedY;
  bool startGameplay;
  bool isStartScreenVisible;

  void updateBallLocation() {
    display.fillCircle(ballPositionX, ballPositionY, 4, Black); 
    ballPositionX += ballSpeedX;
    ballPositionY += ballSpeedY;
    display.fillCircle(ballPositionX, ballPositionY, 4, White);  
  }
  void movePaddle(int direction, int paddleHeight) {
    // 0 - Left
    // 1 - Right
    if (direction == 0) {
      leftButton.drawButton(true);
      display.fillRoundRect(paddlePosition, paddleHeight, 50, 10, 3, Black);
      if (paddlePosition >= 0) {
        paddlePosition -= 20;
      }
    } else {
      rightButton.drawButton(true);
      display.fillRoundRect(paddlePosition, paddleHeight, 50, 10, 3, Black);
      if (paddlePosition <= 190) {
        paddlePosition += 20;
      }
    }
  }
public:
  Game() {
    paddlePosition = 95;
    ballPositionX = 120;
    ballPositionY = 240;
    ballSpeedX = 0;
    ballSpeedY = 0;
    startGameplay = false;
    isStartScreenVisible = true;
  }
};

class PongGame : Game {
private:
  int pongScore;
  int highScore;
  void showScreen(int screenNumber) {
    // 0 --> Start Screen
    // 1 --> Game Screen
    if (screenNumber == 0) {
      ballSpeedX = 0;
      ballSpeedY = 0;
      display.setCursor(60, 140);
      display.setTextColor(Green);
      display.setTextSize(4);
      display.print("START");
    } else {
      display.fillScreen(Black);
      pongScore = 0;
      startGameplay = false;
      ballSpeedX = 1;
      ballSpeedY = -1;
      ballPositionX = 35;
      ballPositionY = 35;
      exitButton.drawButton(false);
      leftButton.drawButton(false);
      rightButton.drawButton(false);
    }
  }
  void handleCollisions() {
    if (ballPositionX < 0) { 
      ballSpeedX = 1;
    } else if (ballPositionX > display.width()) { 
      ballSpeedX = -1;
    } else if (ballPositionY < 50) {  
      ballSpeedY = 1;
    } else if (ballPositionY == 220 && (ballPositionX >= paddlePosition && ballPositionX <= paddlePosition + 50)) { 
      ballSpeedY = -1;
      pongScore++;
      setScore();
    } else if (ballPositionY >= 250) { 
      ballSpeedX = 0;
      ballSpeedY = 0;
      display.setTextColor(Red);
      display.setCursor(15, 100);
      display.setTextSize(4);
      display.print("GAME OVER");
      Serial.write("BUZZ");
      display.setTextSize(3);
      display.setCursor(60, 150);
      display.setTextColor(Green);
      display.print("RESTART");
    }
  }
  void setScore() {
    display.fillRoundRect(30, 10, 40, 40, 3, Black);
    display.setTextSize(3);
    display.setCursor(30, 20);
    display.setTextColor(White);
    display.print(pongScore);

    if (pongScore > highScore) {
      highScore = pongScore;
      EEPROM.write("high_score", highScore);
    }
  }
  void exit() {
    isStartScreenVisible = true;
    isPongSelected = false;
    ballPositionX = 35;
    ballPositionY = 35;
    // to ensure restart and start text does not overlap next time pong is opened

    display.fillScreen(Black);
  }

public:
  PongGame() {
    pongScore = 0;
    highScore = EEPROM.read("high_score");
    if (highScore == 255)
      highScore = 0;  // eeprom gives 255 when address was never written
  }
  void pong() {
    if (isStartScreenVisible) {
      showScreen(0);
    }
    if (startGameplay) {  
      showScreen(1);

      display.setTextSize(2);
      display.setCursor(70, 10);
      display.setTextColor(White);
      display.print("High Score: ");
      display.print(highScore);
    } else {
      display.fillRoundRect(0, 50, 400, 3, 3, Red);
      display.fillRoundRect(0, 260, 400, 3, 3, Red);
      updateBallLocation();
      handleCollisions();
      display.fillRoundRect(paddlePosition, 220, 50, 10, 3, Cyan);
    }

    bool isTouched = utility.isScreenTouched();

    leftButton.press(isTouched && leftButton.contains(pixelX, pixelY));
    rightButton.press(isTouched && rightButton.contains(pixelX, pixelY));
    startButton.press(isTouched && startButton.contains(pixelX, pixelY));
    exitButton.press(isTouched && exitButton.contains(pixelX, pixelY));

    if (leftButton.justReleased())
      leftButton.drawButton();
    if (rightButton.justReleased())
      rightButton.drawButton();

    if (startButton.justPressed()) {
      startGameplay = true;
      isStartScreenVisible = false;
    }
    char command;

    if (Serial3.available() > 0) {
    command = Serial3.read(); 
    }

    if (leftButton.justPressed() || command == 'L')
      movePaddle(0, 220);
    if (rightButton.justPressed() || command =='R')
      movePaddle(1, 220);
    if (exitButton.justPressed())
      exit();
  
   
  }
};

class BreakoutGame : Game {
private:
  int poppedRedBricks;   
  int poppedOrangeBricks;   
  int poppedYellowBricks; 
  int poppedGreenBricks;   
  void resetBricks() {
    poppedRedBricks = 0;
    poppedOrangeBricks = 0;
    poppedYellowBricks = 0;
    poppedGreenBricks = 0;
  }
  void showScreen(int screenNumber) {
    // 0 --> start screen
    // 1 --> gameplay screen
    if (screenNumber == 0) {
      display.setTextSize(4);
      display.setCursor(60, 140);
      display.setTextColor(Green);
      display.print("START");
    } else {
      display.fillScreen(Black);
      utility.drawBreakoutPattern();
      ballSpeedX = 1;
      ballSpeedY = -1;
      startGameplay = false;
      exitButton.drawButton(false);
      leftButton.drawButton(false);
      rightButton.drawButton(false);
    }
  }
  void updateBricks() {
    for (int j = 0; j < 4; j++) {
      int reqY = (j + 4) * 10 + j;
      int currentY = (j + 3) * 10 + j;
      if ((j == 0 && poppedGreenBricks < 4) || (j == 1 && poppedYellowBricks < 4) || (j == 2 && poppedOrangeBricks < 4) || (j == 3 && poppedRedBricks < 4)) {
        for (int i = 0; i < 4; i++) {
          int currentX = 60 * i;
          if (ballPositionX >= currentX && ballPositionX <= currentX + 60 && ballPositionY == reqY) {  
            display.fillRoundRect(currentX, currentY, 59, 10, 3, Black);
            ballSpeedY = 1;
            if (j == 0)
              poppedGreenBricks++;
            else if (j == 1)
              poppedYellowBricks++;
            else if (j == 2)
              poppedOrangeBricks++;
            else if (j == 3)
              poppedRedBricks++;
          }
        }
      }
    }
  }
  void handleCollisions() {
    if (ballPositionX < 0) {  
      ballSpeedX = 1;
    } else if (ballPositionX > display.width()) { 
      ballSpeedX = -1;
    } else if (ballPositionY < 0) {
      ballSpeedY = 1;
    } else if (ballPositionY == 260 && (ballPositionX >= paddlePosition && ballPositionX <= paddlePosition + 50)) { 
      ballSpeedY = -1;
    } else if (ballPositionY == 280) { 
      display.fillScreen(Black);
      exitButton.drawButton(false);
      ballSpeedX = 0;
      ballSpeedY = 0;
      resetBricks();

      ballPositionY = 240;
      ballPositionX = 120;

      display.setTextSize(4);
      display.setCursor(40, 140);
      display.setTextColor(Green);
      display.print("RESTART");
      Serial.print("BUZZ");
      display.setTextSize(4);
      display.setCursor(15, 80);
      display.setTextColor(Red);
      display.print("GAME OVER");
    }
  }
  void exit() {
    display.fillScreen(Black);
    isBreakoutSelected = false;
    resetBricks();
    isStartScreenVisible = true;
    ballSpeedX = 0;
    ballSpeedY = 0;
  }

public:
  BreakoutGame() {
    resetBricks();
  }
  void breakout() {
    if (isStartScreenVisible) {
      showScreen(0);
    }
    if (startGameplay) {  
      showScreen(1);
    } else {
      updateBricks();
      updateBallLocation();
      handleCollisions();
      display.fillRoundRect(paddlePosition, 260, 50, 10, 3, Cyan);  
      if (poppedGreenBricks == 5) {                                           
        display.fillScreen(Black);
        display.setTextSize(3);
        display.setCursor(40, 140);
        display.setTextColor(Green);
        display.print("Congrats!");
        Serial.print("BUZZ");

        delay(2000);
        poppedGreenBricks = 0;
        exit();
      }
    }

    bool isTouched = utility.isScreenTouched();
    leftButton.press(isTouched && leftButton.contains(pixelX, pixelY));
    rightButton.press(isTouched && rightButton.contains(pixelX, pixelY));
    startButton.press(isTouched && startButton.contains(pixelX, pixelY));
    exitButton.press(isTouched && exitButton.contains(pixelX, pixelY));

    if (leftButton.justReleased())
      leftButton.drawButton();
    if (rightButton.justReleased())
      rightButton.drawButton();

    if (startButton.justPressed()) {
      isStartScreenVisible = false;
      startGameplay = true;
    }
    char command;

     if (Serial3.available() > 0) {
    command = Serial3.read(); 
    }

    if (leftButton.justPressed() || command == 'L')
      movePaddle(0, 220);
    if (rightButton.justPressed() || command =='R')
      movePaddle(1, 220);
    if (exitButton.justPressed())
      exit();
    if (exitButton.justPressed())
      exit();
  }
};

BreakoutGame breakoutGame;
PongGame pongGame;

void loop() {
  utility.monitorHomeScreenClicks();
  if (!isPongSelected && !isBreakoutSelected) {
    utility.showHomeScreen();
  }
  if (isPongSelected) {
    pongGame.pong();
  }
  if (isBreakoutSelected) {
    breakoutGame.breakout();
  }
}