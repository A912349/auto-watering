#include "wifi_manager.h"
#include <WebServer.h>
#include "storage.h"
#include <time.h>

// внешние переменные
extern String wifi_ssid;
extern String wifi_pass;
extern int timezone;
extern int threshold;
extern int waterTime;
extern bool autoMode;

WebServer server(80);

bool apMode = false;
bool timeInitialized = false;

String getIP() {
  if (apMode) {
    return WiFi.softAPIP().toString();
  } else if (WiFi.status() == WL_CONNECTED) {
    return WiFi.localIP().toString();
  }
  return "No IP";
}

String getSSID() {
  if (apMode) return "ESP32-Setup";
  return wifi_ssid;
}

void initTime() {
  configTime(timezone * 3600, 0, "pool.ntp.org", "ru.pool.ntp.org");
}

String getTimeStr() {
  if (WiFi.status() != WL_CONNECTED) return "--:--";

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 10)) return "sync...";

  char buf[20];
  strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo);
  return String(buf);
}

bool isAPMode() {
  return apMode;
}

void startAP() {
  WiFi.softAP("ESP32-Setup", "12345678");
  apMode = true;

  server.on("/", []() {
    String page = "<form action='/save'>";
    page += "SSID:<input name='s'><br>";
    page += "PASS:<input name='p'><br>";
    page += "TZ:<input name='t'><br>";
    page += "<input type='submit'>";
    page += "</form>";
    server.send(200, "text/html", page);
  });

  server.on("/save", []() {
    wifi_ssid = server.arg("s");
    wifi_pass = server.arg("p");
    timezone = server.arg("t").toInt();
    
    saveSettings();
    
    server.send(200, "text/html", "Saved. Reboot.");
  });

  server.begin();
}

void wifiInit() {
  if (wifi_ssid != "") {
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
  }
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    tries++;
  }
}

void wifiLoop() {
if (apMode) {
    server.handleClient();
    return;
  }

  // если подключились к Wi-Fi — запускаем NTP один раз
  if (!timeInitialized && WiFi.status() == WL_CONNECTED) {
    initTime();
    timeInitialized = true;
  }
}