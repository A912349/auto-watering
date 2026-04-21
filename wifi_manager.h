#pragma once

#include <WiFi.h>

void wifiInit();
void wifiLoop();
void startAP();
String getIP();
String getSSID();
bool isAPMode();
String getTimeStr();
void handleRoot();
void handleData();
void handleCmd();
void handleSave();
void handleSet();

extern bool apMode;
