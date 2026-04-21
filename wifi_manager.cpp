#include "wifi_manager.h"
#include <WebServer.h>
#include "storage.h"
#include <time.h>

String htmlPage(bool showSetup) {

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Auto Watering</title>

<style>
body {
  font-family: Arial;
  background: #0f172a;
  color: white;
  text-align: center;
}

.card {
  background: #1e293b;
  padding: 15px;
  margin: 10px;
  border-radius: 12px;
}

button {
  padding: 10px;
  margin: 5px;
  border-radius: 8px;
  border: none;
  background: #3b82f6;
  color: white;
}

input {
  padding: 6px;
  width: 80px;
  text-align: center;
}
</style>

</head>
<body>

<h2>Auto Watering</h2>

<div class="card">
<p>Moisture: <b id="m">--</b></p>
<p>Pump: <b id="p">--</b></p>
</div>

<div class="card">
<h3>Control</h3>

<p>Threshold</p>
<button onclick="change('thr',-100)">-</button>
<span id="thr">--</span>
<button onclick="change('thr',100)">+</button>

<p>Water time (sec)</p>
<button onclick="change('wt',-1)">-</button>
<span id="wt">--</span>
<button onclick="change('wt',1)">+</button>

<p>Interval (x15 min)</p>
<button onclick="change('int',-1)">-</button>
<span id="int">--</span>
<button onclick="change('int',1)">+</button>

<p>Auto mode</p>
<button onclick="cmd('auto')">Toggle</button>
<span id="auto">--</span>

<br><br>
<button onclick="cmd('pump')">Start Pump</button>

</div>
)rawliteral";

  if (showSetup) {
    html += R"rawliteral(
<div class="card">
<h3>WiFi Setup</h3>
<form action="/save">
<input name="s" placeholder="SSID"><br><br>
<input name="p" placeholder="Password"><br><br>
<input name="t" placeholder="Timezone"><br><br>
<button type="submit">Save</button>
</form>
</div>
)rawliteral";
  }

  html += R"rawliteral(

<script>

function load(){
 fetch('/data')
 .then(r=>r.json())
 .then(d=>{
   document.getElementById('m').innerText = d.moisture;
   document.getElementById('p').innerText = d.pump ? "ON":"OFF";

   document.getElementById('thr').innerText = d.threshold;
   document.getElementById('wt').innerText = d.waterTime;
   document.getElementById('int').innerText = d.interval;
   document.getElementById('auto').innerText = d.auto ? "ON":"OFF";
 });
}

function cmd(c){
 fetch('/cmd?c='+c).then(load);
}

function change(type,val){
 fetch('/set?type='+type+'&v='+val).then(load);
}

setInterval(load, 2000);
load();

</script>

</body>
</html>
)rawliteral";

  return html;
}

// внешние переменные
extern String wifi_ssid;
extern String wifi_pass;
extern int timezone;
extern int threshold;
extern int waterTime;
extern bool autoMode;
extern int threshold;
extern int waterTime;
extern bool autoMode;
extern int waterInterval;
extern int moisture;
extern bool pumpActive;

WebServer server(80);

bool apMode = false;
bool timeInitialized = false;

void handleRoot() {
  server.send(200, "text/html", htmlPage(apMode));
}

void handleData() {
  String json = "{";
  json += "\"moisture\":" + String(moisture) + ",";
  json += "\"threshold\":" + String(threshold) + ",";
  json += "\"waterTime\":" + String(waterTime) + ",";
  json += "\"interval\":" + String(waterInterval) + ",";
  json += "\"pump\":" + String(pumpActive ? 1 : 0) + ",";
  json += "\"auto\":" + String(autoMode ? 1 : 0);
  json += "}";

  server.send(200, "application/json", json);
}

void handleCmd() {
  String c = server.arg("c");

  if (c == "pump") {
    extern void startPump();
    startPump();
  }

  if (c == "auto") {
    autoMode = !autoMode;
    saveSettings();
  }

  server.send(200, "text/plain", "OK");
}

void handleSave() {
  wifi_ssid = server.arg("s");
  wifi_pass = server.arg("p");
  timezone = server.arg("t").toInt();

  saveSettings();

  server.send(200, "text/html", "Saved. Reboot device.");
}

void handleSet() {

  String type = server.arg("type");
  int v = server.arg("v").toInt();

  if (type == "thr") {
    threshold += v;
    if (threshold < 0) threshold = 0;
    if (threshold > 4095) threshold = 4095;
  }

  if (type == "wt") {
    waterTime += v;
    if (waterTime < 1) waterTime = 1;
    if (waterTime > 60) waterTime = 60;
  }

  if (type == "int") {
    waterInterval += v;
    if (waterInterval < 0) waterInterval = 0;
    if (waterInterval > 40) waterInterval = 40;
  }

  saveSettings();

  server.send(200, "text/plain", "OK");
}

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

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/cmd", handleCmd);
  server.on("/save", handleSave);
  server.on("/set", handleSet);

  server.begin();
}

void wifiInit() {

  WiFi.mode(WIFI_STA);

  if (wifi_ssid != "") {
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
  }

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    tries++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    startAP();
  } else {
    apMode = false;

    server.on("/", handleRoot);
    server.on("/data", handleData);
    server.on("/cmd", handleCmd);
    server.on("/set", handleSet);

    server.begin();
  }
}

void wifiLoop() {
  server.handleClient();

  if (!apMode && !timeInitialized && WiFi.status() == WL_CONNECTED) {
    initTime();
    timeInitialized = true;
  }
}
