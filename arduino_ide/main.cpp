#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

static const char* WIFI_SSID = "nome-wifi";
static const char* WIFI_PASS = "senha-wifi";
static const char* BASE_URL  = "http://192.168.2.1:5000";

// Pins
static const int PIN_LED = 25; // LED + resistor
static const int PIN_PIR = 26; // PIR OUT
static const int PIN_LDR = 34; // LDR AO (ADC1)

// Auto
static const int LDR_THRESHOLD = 1800;
static const unsigned long OFF_DELAY_MS = 7000;

// WiFi tentativa de reconexão (tempo)
static const unsigned long WIFI_RETRY_MS = 10000;

// Intervalos
static const unsigned long TELEMETRY_MS = 1000;
static const unsigned long COMMAND_MS   = 500;
static const unsigned long DEBUG_MS     = 500;

enum class Mode { Auto, Manual };

// Estado local
bool lampOn = false;
bool pir = false;
int ldr = 0;

unsigned long lastMotionMs = 0;
bool motionEverDetected = false;
unsigned long lastTelemetryMs = 0;
unsigned long lastCommandMs = 0;
unsigned long lastDebugMs = 0;
unsigned long lastWifiRetryMs = 0;

bool prevPir = false;
bool prevLampOn = false;

// Estado vindo do backend
Mode mode = Mode::Auto;
bool manualLampOn = false;

void setLamp(bool on) {
  lampOn = on;
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}

// Tenta conectar no wifi
void connectWiFi(unsigned long now) {
  if (WiFi.status() == WL_CONNECTED) return;

  if (now - lastWifiRetryMs < WIFI_RETRY_MS) return;
  lastWifiRetryMs = now;

  Serial.println("WiFi desconectado, tentando reconectar...");
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void readSensors(unsigned long now) {
  pir = (digitalRead(PIN_PIR) == HIGH);
  ldr = analogRead(PIN_LDR);
  if (pir) { lastMotionMs = now; motionEverDetected = true; }
}

void applyAutoRule(unsigned long now) {
  const bool isDark = (ldr > LDR_THRESHOLD);
  const bool recentlyMotion = motionEverDetected && (now - lastMotionMs) < OFF_DELAY_MS;
  const bool target = isDark && recentlyMotion;
  if (target != lampOn) setLamp(target);
}

void applyManualRule() {
  if (manualLampOn != lampOn) setLamp(manualLampOn);
}

void fetchCommand(unsigned long now) {
  if (now - lastCommandMs < COMMAND_MS) return;
  lastCommandMs = now;

  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  String url = String(BASE_URL) + "/api/command";
  http.begin(url);

  int code = http.GET();
  if (code <= 0) {
    Serial.print("GET /api/command falhou: ");
    Serial.println(code);
    http.end();
    return;
  }

  String payload = http.getString();
  http.end();

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    Serial.print("JSON comando inválido: ");
    Serial.println(err.c_str());
    return;
  }

  const char* modeStr = doc["mode"] | "auto";
  if (strcmp(modeStr, "manual") == 0) mode = Mode::Manual;
  else mode = Mode::Auto;

  manualLampOn = doc["manualLampOn"] | false;
}

void sendTelemetry(unsigned long now, bool force) {
  if (!force && now - lastTelemetryMs < TELEMETRY_MS) return;
  lastTelemetryMs = now;

  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  String url = String(BASE_URL) + "/api/telemetry";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  JsonDocument doc;
  doc["pir"] = pir;
  doc["ldr"] = ldr;
  doc["lampOn"] = lampOn;
  doc["mode"] = (mode == Mode::Manual) ? "manual" : "auto";
  doc["uptimeMs"] = (uint32_t)now;

  String body;
  serializeJson(doc, body);

  int code = http.POST(body);
  Serial.print("POST /api/telemetry -> ");
  Serial.println(code);

  http.end();
}

void debugPrint(unsigned long now) {
  if (now - lastDebugMs < DEBUG_MS) return;
  lastDebugMs = now;

  Serial.print("wifi=");
  Serial.print(WiFi.status() == WL_CONNECTED ? "ok" : "off");
  Serial.print(" mode=");
  Serial.print(mode == Mode::Manual ? "manual" : "auto");
  Serial.print(" pir=");
  Serial.print(pir ? 1 : 0);
  Serial.print(" ldr=");
  Serial.print(ldr);
  Serial.print(" lampOn=");
  Serial.print(lampOn ? 1 : 0);
  Serial.print(" manualLampOn=");
  Serial.println(manualLampOn ? 1 : 0);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_PIR, INPUT_PULLDOWN);

  setLamp(false);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando no WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi OK. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi falhou, continuando sem WiFi.");
  }

  Serial.println("Iniciado.");
}

void loop() {
  unsigned long now = millis();

  connectWiFi(now);

  readSensors(now);
  fetchCommand(now);

  if (mode == Mode::Manual) applyManualRule();
  else applyAutoRule(now);

  bool changed = (pir != prevPir) || (lampOn != prevLampOn);
  prevPir = pir;
  prevLampOn = lampOn;

  debugPrint(now);
  sendTelemetry(now, changed);

  delay(10);
}
