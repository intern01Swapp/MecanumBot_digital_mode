// File: esp32_bridge.ino
#include <Arduino.h>

// File: esp32_bridge.ino
#include <Arduino.h>
// ---------- ESP32 – Wi-Fi TCP bridge ----------
#include <WiFi.h>

// Wi-Fi
const char* ssid = "SD_2.4G";
const char* password = "29603999";

// TCP Server on port 3333
WiFiServer server(3333);
WiFiClient client;

// Serial2 → Mega
#define MEGA_RX 16 // ESP32 RX ← Mega TX (via level shifter)
#define MEGA_TX 17 // ESP32 TX → Mega RX

// Blue LED (built-in on most ESP32 boards)
#define BLUE_LED 2
bool ledState = false;
unsigned long lastBlink = 0;
const long blinkInterval = 500; // ms

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, MEGA_RX, MEGA_TX);

  // Initialize LED
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.println("IP: " + WiFi.localIP().toString());

  server.begin();
  server.setNoDelay(true);
  Serial.println("Connect: nc " + WiFi.localIP().toString() + " 3333");
}

void loop() {
  // Accept new client
  if (!client && server.hasClient()) {
    client = server.available();
    client.println("ESP32 Bridge Ready");
    Serial.println("Client connected");
  }

  // === PC → ESP32 → Mega ===
  if (client && client.connected()) {
    while (client.available()) {
      char b = client.read();
      if (b == '\r' || b == '\n') continue; // ignore CR/LF
      Serial2.write(b);
      delay(2);
    }
  }

  // === Mega → ESP32 → PC ===
  while (Serial2.available()) {
    char c = Serial2.read();
    if (client && client.connected()) {
      client.write(c);
    }
    Serial.write(c); // debug
  }

  // Auto-close if disconnected
  if (client && !client.connected()) {
    if (client) {
      client.stop();
      Serial.println("Client disconnected");
    }
  }

  // === Blink Blue LED only when client is connected ===
  if (client && client.connected()) {
    unsigned long now = millis();
    if (now - lastBlink >= blinkInterval) {
      ledState = !ledState;
      digitalWrite(BLUE_LED, ledState ? HIGH : LOW);
      lastBlink = now;
    }
  } else {
    // Turn off LED when no client
    digitalWrite(BLUE_LED, LOW);
  }
}