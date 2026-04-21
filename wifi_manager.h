#pragma once

#include <WiFi.h>

void wifiInit();
void wifiLoop();
void startAP();
String getIP();
String getSSID();
bool isAPMode();
String getTimeStr();

extern bool apMode;
