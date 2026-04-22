#include "ui.h"
#include "wifi_manager.h"
#include "storage.h"

#define Pump      19
#define Sensor    34

#define BTN_LEFT  25
#define BTN_RIGHT 26


// ===== состояния =====
int screen = 0;
int menuIndex = 0;


// ===== датчик =====
int moisture = 0;

// ===== полив =====
bool pumpActive = false;
unsigned long pumpStart = 0;
unsigned long lastWaterTime = 0;

// ===== debounce =====
bool stableLeft = HIGH;
bool stableRight = HIGH;
unsigned long lastDebounceLeft = 0;
unsigned long lastDebounceRight = 0;
const int debounceDelay = 60;

// ===== таймер =====
unsigned long lastUpdate = 0;


// ===== логика =====

void startPump() {
  pumpActive = true;
  pumpStart = millis();
  digitalWrite(Pump, HIGH);
}

void stopPump() {
  pumpActive = false;
  digitalWrite(Pump, LOW);
}

// ===== setup =====

void setup() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);

  pinMode(Pump, OUTPUT);
  pinMode(Sensor, INPUT);

  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);

  digitalWrite(Pump, LOW);

  loadSettings();
  wifiInit();
  updateScreen(screen);
}

// ===== loop =====

void loop() {

  wifiLoop();
  // датчик
  if (millis() - lastUpdate > 500) {
    moisture = analogRead(Sensor);
    lastUpdate = millis();

    if (screen == 0) drawMain();
  }

  // автополив
  if (autoMode && !pumpActive && moisture < threshold) {
    unsigned long intervalMs = waterInterval * 15UL * 60UL * 1000UL;

    if (autoMode && !pumpActive && moisture < threshold) {
      if (millis() - lastWaterTime > intervalMs) {
        startPump();
        lastWaterTime = millis();
      }
    }
  }

  // остановка по времени
  if (pumpActive && millis() - pumpStart > waterTime * 1000) {
    stopPump();
  }

  bool readLeft = digitalRead(BTN_LEFT);
  bool readRight = digitalRead(BTN_RIGHT);
  // ===== кнопки =====
  bool clickLeft = false;
  bool clickRight = false;
  
  // ===== debounce LEFT =====
  if (readLeft != stableLeft) {
    if (millis() - lastDebounceLeft > debounceDelay) {
      stableLeft = readLeft;
  
      if (stableLeft == LOW) {
        clickLeft = true;
      }
    }
    lastDebounceLeft = millis();
  }
  
  // ===== debounce RIGHT =====
  if (readRight != stableRight) {
    if (millis() - lastDebounceRight > debounceDelay) {
      stableRight = readRight;
  
      if (stableRight == LOW) {
        clickRight = true;
      }
    }
    lastDebounceRight = millis();
  }
  
  // ===== обработка LEFT =====
  if (clickLeft) {
    clickLeft = false;
  
    if (screen == 0) {
      screen = 1;
    }
    else if (screen == 1) {
      menuIndex++;
      if (menuIndex > 6) menuIndex = 0;
    }
    else {
      screen = 1;
    }
  
    updateScreen(screen);
  }
  
  // ===== обработка RIGHT =====
  if (clickRight) {
    clickRight = false;
  

    if (screen == 1 && menuIndex == 6) {
      screen = 0;
      updateScreen(screen);
    }
    else if (screen == 1) {
      screen = 2 + menuIndex;
      updateScreen(screen);
    }

  
    else if (screen == 2) {
      threshold += 100;
      if (threshold > 4000) threshold = 0;
      drawThreshold();
      saveSettings();
    }
  
    else if (screen == 3) {
      waterTime++;
      if (waterTime > 30) waterTime = 1;
      drawWaterTime();
      saveSettings();
    }

    else if (screen == 4) {
      waterInterval++;
      if (waterInterval > 20) waterInterval = 0;
      saveSettings();
      drawInterval();
    }
  
    else if (screen == 5) {
      autoMode = !autoMode;
      drawAuto();
      saveSettings();
    }
  
    else if (screen == 6) {
      startPump();
    }
    else if (screen == 7) {
      startAP();
      drawWifi();
    }
  }
}