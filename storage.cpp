#include "storage.h"
#include <Preferences.h>

Preferences prefs;

// ===== реальные переменные =====
int threshold = 2500;
int waterTime = 5;
bool autoMode = true;

String wifi_ssid = "";
String wifi_pass = "";
int timezone = 0;
int waterInterval = 1;

// ===== загрузка =====
void loadSettings() {
  prefs.begin("settings", true);

  threshold = prefs.getInt("thr", 2500);
  waterTime = prefs.getInt("wt", 5);
  autoMode = prefs.getBool("auto", true);
  waterInterval = prefs.getInt("wint", 1);

  wifi_ssid = prefs.getString("ssid", "");
  wifi_pass = prefs.getString("pass", "");
  timezone = prefs.getInt("tz", 0);


  prefs.end();
}

// ===== сохранение =====
void saveSettings() {
  prefs.begin("settings", false);

  prefs.putInt("thr", threshold);
  prefs.putInt("wt", waterTime);
  prefs.putBool("auto", autoMode);

  prefs.putString("ssid", wifi_ssid);
  prefs.putString("pass", wifi_pass);
  prefs.putInt("tz", timezone);
  prefs.putInt("wint", waterInterval);

  prefs.end();
}