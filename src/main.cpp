#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <main.h>

#define OLED_D1     11 // MOSI
#define OLED_D0     13 // CLK
#define OLED_DC     12
#define OLED_CS     10
#define OLED_RESET  14
Adafruit_SSD1306 display(OLED_D1, OLED_D0, OLED_DC, OLED_RESET, OLED_CS);

#define CLOCK_IN    18
#define POT1_IN     19
#define BUT1_IN     9

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

#define STEP_HEIGHT 16
#define COL_NUM 4
#define COL_WIDTH 16

#define DISPLAY_HEIGHT 64
#define DISPLAY_WIDTH 128

#define SSD1306_LCDHEIGHT 64

int activeSteps[COL_NUM] = {0, 0, 0, 0};
boolean activeTriggers[COL_NUM] = {false, false, false, false};
int colHeights[COL_NUM] = {0, 0, 0, 0};
boolean colEditable[COL_NUM] = {false, false, false, false};
int colTriggerTargets[COL_NUM] = {0, 1, 2, 3};

void activateTrigger(int colNum) {
  activeTriggers[colNum] = true;
}

void fireTriggers() {
  //Serial.println("triggering: " + String(activeTriggers[0]) + String(activeTriggers[1]) + String(activeTriggers[2]) + String(activeTriggers[3]));
}

void resetTriggers() {
  for(int i=0; i < COL_NUM; i++) {
      activeTriggers[i] = false;
  }
}

void initCol(int colNum, int height) {
  int y = colNum * COL_WIDTH;
  for(int i=0; i < height; i++) {
      int x = i * STEP_HEIGHT;
      display.drawRect(x, y, 14, 14, 1);
  }
  display.fillRect(height * STEP_HEIGHT, y, 14, 14, 1);
  activeSteps[colNum] = height;
  colHeights[colNum] = height;
}

void updateCol(int colNum) {
  if(!isInEditMode(colNum)) {
    int x = activeSteps[colNum] * STEP_HEIGHT;
    int y = colNum * COL_WIDTH;
    display.fillRect(x, y, 14, 14, 0);
    display.drawRect(x, y, 14, 14, 1);

    if(activeSteps[colNum] == 0) {
      activeSteps[colNum] = colHeights[colNum] + 1;
      activateTrigger(colTriggerTargets[colNum]);
    }

    int new_x = (activeSteps[colNum] - 1) * STEP_HEIGHT;
    display.fillRect(new_x, y, 14, 14, 1);
    activeSteps[colNum] -= 1;
  }
}

void blankCol(int colNum) {
  int x = 0;
  int y = colNum * COL_WIDTH;
  display.fillRect(x, y, DISPLAY_WIDTH, COL_WIDTH, 0);
}

void drawEmptyCol(uint8_t colNum, uint8_t height) {
  int y = colNum * COL_WIDTH;
  for(int i=0; i < height; i++) {
      int x = i * STEP_HEIGHT;
      display.drawRect(x, y, 14, 14, 1);
  }
}

boolean isInEditMode(int colNum) {
  return colEditable[colNum];
}

void onClockReceived() {
  updateCol(0);
  updateCol(1);
  updateCol(2);
  updateCol(3);
  fireTriggers();
  resetTriggers();
  display.display();
  Serial.println("CLOCK IN");
}

void onButton1Pressed() {
  uint8_t colNum = 0;

  Serial.println("BUTTON 1 pressed");
  colEditable[colNum] = !colEditable[colNum];
  blankCol(colNum);
}

void setup()   {
  Serial.begin(9600);

  pinMode(CLOCK_IN, INPUT);
  pinMode(BUT1_IN, INPUT);

  attachInterrupt(CLOCK_IN, onClockReceived, RISING);
  attachInterrupt(BUT1_IN, onButton1Pressed, FALLING);


  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);

  // Clear the buffer.
  display.clearDisplay();

  // draw multiple rectangles
  //testfillrect();
  initCol(0, 3);
  initCol(1, 7);
  initCol(2, 6);
  initCol(3, 2);
  display.display();
}

void loop() {
  int pot1_val = analogRead(POT1_IN);
  Serial.println(pot1_val);

  onClockReceived();
  delay(pot1_val);
}
