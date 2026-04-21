#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// внешний доступ к TFT
extern Adafruit_ST7735 tft;

// переменные из main
extern int moisture;
extern int threshold;
extern int waterTime;
extern bool autoMode;
extern bool pumpActive;
extern int menuIndex;

// функции
void drawMain();
void drawMenu();
void drawThreshold();
void drawWaterTime();
void drawInterval();
void drawAuto();
void drawManual();
void drawWifi();
void updateScreen(int screen);