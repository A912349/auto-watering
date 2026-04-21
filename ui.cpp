#include "ui.h"
#include "wifi_manager.h"
#include "storage.h"

extern String getIP();
// TFT создаём тут
Adafruit_ST7735 tft = Adafruit_ST7735(5, 2, 4);

const char* menuItems[] = {
  "Threshold",
  "Water time",
  "Water interval",
  "Auto mode",
  "Manual water",
  "WiFi",
  "Exit"
};

void drawMain() {
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(1);
  tft.setCursor(0,0);
  tft.setTextColor(ST77XX_WHITE);
  tft.println("MAIN");

  tft.setTextSize(2);
  tft.setCursor(0,20);
  tft.setTextColor(ST77XX_GREEN);
  tft.print("ADC: ");
  tft.println(moisture);

  tft.setTextSize(1);
  tft.setCursor(0,60);
  tft.print("AUTO: ");
  tft.println(autoMode ? "ON" : "OFF");

  tft.setCursor(0,70);
  tft.print("THR: ");
  tft.println(threshold);

  tft.setCursor(0,80);
  tft.print("PUMP: ");
  tft.println(pumpActive ? "ON" : "OFF");

  tft.setCursor(0, 100);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_CYAN);
  tft.println(getIP());
  tft.setCursor(0, 120);
  tft.setTextColor(ST77XX_CYAN);
  tft.println(getTimeStr());
}

void drawMenu() {
  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(0,0);
  tft.setTextColor(ST77XX_WHITE);
  tft.println("MENU");

  for (int i = 0; i < 7; i++) {
    tft.setCursor(0, 15 + i*12);

    if (i == menuIndex) {
      tft.setTextColor(ST77XX_YELLOW);
      tft.print("> ");
    } else {
      tft.setTextColor(ST77XX_WHITE);
      tft.print("  ");
    }

    tft.println(menuItems[i]);
  }
}

void drawThreshold() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0,0);
  tft.println("Threshold");

  tft.setTextSize(3);
  tft.setCursor(0,30);
  tft.println(threshold);

  tft.setTextSize(1);
  tft.setCursor(0,80);
  tft.println("RIGHT:+100");
}

void drawWaterTime() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0,0);
  tft.println("Water sec");

  tft.setTextSize(3);
  tft.setCursor(0,30);
  tft.println(waterTime);

  tft.setTextSize(1);
  tft.setCursor(0,80);
  tft.println("RIGHT:+1");
}

void drawInterval() {
  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(0,0);
  tft.setTextSize(1);
  tft.println("Interval: ");

  tft.setTextSize(3);
  tft.setCursor(0,30);
  tft.print(waterInterval * 15);
  tft.print(" mins");

  tft.setTextSize(1);
  tft.setCursor(0,80);
  tft.println("RIGHT:+1");
}

void drawAuto() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0,0);
  tft.println("Auto mode");

  tft.setTextSize(3);
  tft.setCursor(0,30);
  tft.println(autoMode ? "ON" : "OFF");

  tft.setTextSize(1);
}

void drawManual() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0,0);
  tft.println("Manual water");

  tft.setCursor(0,40);
  tft.println("RIGHT = START");
}

void drawWifi() {
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(1);
  tft.setCursor(0,0);
  tft.setTextColor(ST77XX_WHITE);
  tft.println("WiFi");

  tft.setCursor(0,15);

  if (isAPMode()) {
    tft.println("MODE: AP");

    tft.print("SSID: ");
    tft.println(getSSID());

    tft.print("PASS: ");
    tft.println("12345678");

    tft.print("IP: ");
    tft.println(getIP());
  } else {
    tft.println("MODE: STA");

    tft.print("SSID: ");
    tft.println(getSSID());

    tft.print("IP: ");
    tft.println(getIP());
  }

  tft.setCursor(0, 100);
  tft.println("RIGHT = START AP");
}

void updateScreen(int screen) {
  switch(screen) {
    case 0: drawMain(); break;
    case 1: drawMenu(); break;
    case 2: drawThreshold(); break;
    case 3: drawWaterTime(); break;
    case 4: drawInterval(); break;
    case 5: drawAuto(); break;
    case 6: drawManual(); break;
    case 7: drawWifi(); break;
  }
}