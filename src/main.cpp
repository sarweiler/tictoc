#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_D1     11 // MOSI
#define OLED_D0     13 // CLK
#define OLED_DC     12
#define OLED_CS     10
#define OLED_RESET  14
Adafruit_SSD1306 display(OLED_D1, OLED_D0, OLED_DC, OLED_RESET, OLED_CS);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

#define STEP_HEIGHT 16
#define COL_NUM 4
#define COL_WIDTH 16

#define SSD1306_LCDHEIGHT 64

int activeSteps[COL_NUM] = {0, 0, 0, 0};
int activeTriggers[COL_NUM] = {false, false, false, false};
int colHeights[COL_NUM] = {0, 0, 0, 0};
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

void setup()   {
  Serial.begin(9600);

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
  updateCol(0);
  updateCol(1);
  updateCol(2);
  updateCol(3);
  fireTriggers();
  resetTriggers();

  display.display();
  delay(500);
}
