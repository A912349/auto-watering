#pragma once

#include <Arduino.h>

// ===== переменные (общие) =====
extern int threshold;
extern int waterTime;
extern bool autoMode;
extern int waterInterval;

extern String wifi_ssid;
extern String wifi_pass;
extern int timezone;

// ===== функции =====
void loadSettings();
void saveSettings();