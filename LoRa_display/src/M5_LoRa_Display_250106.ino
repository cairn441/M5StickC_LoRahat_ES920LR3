/********************************************************************************
*
* LoRa    : ES920LR3
* receive data display
*
********************************************************************************/

#define ROT 3
#define TSIZE 3
#define RPOS 100
#define SX 210
#define SY 30
#define SR 12
#define RX_pin 26
#define TX_pin 0
#define RESET_pin -1
#define BOOT_pin -1

#include <M5Unified.h>
#include <Arduino.h>
#include "LoRa.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>

const char* ssid= "Buffalo-G-C630";//【★変更要★】
const char* password="na4uxed55fbvv";//【★変更要★】

// Google Apps Script Web App Key
const char* KEY = "AKfycbxtjmda5jAuTanr29nIWuPhw7YhOT41QhRr1LLKcobVPPKn4qIIqtmPPtz5IY4hNnk5"; // ★ Change if needed

void accessToGoogleSheets(String UniqueID) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected.");
    return;
  }

  HTTPClient http;
  String URL = "https://script.google.com/macros/s/";
  URL += KEY;
  URL += "/exec?UniqueID=";
  URL += UniqueID;

  http.begin(URL);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String response = http.getString();
    Serial.println("Response: " + response);
  } else {
    Serial.printf("HTTP Error: %d\n", httpCode);
  }
  http.end();
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  LoRaInit(RX_pin, TX_pin, RESET_pin, BOOT_pin);

  M5.Lcd.setRotation(ROT);
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.println("Booting...");
  delay(1000);
  M5.Lcd.fillScreen(BLACK);

  // Wi-Fi connection
  WiFi.disconnect(true, true);
  delay(1000);
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 20000) {
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    M5.Lcd.println("WiFi Connected!");
    M5.Lcd.print("IP: ");
    M5.Lcd.println(WiFi.localIP());
  } else {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.println("WiFi Connection Failed!");
    M5.Lcd.setTextColor(WHITE);
  }

  // Set NTP (Japan time zone)
  configTime(9 * 3600, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp", "pool.ntp.org");
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    M5.Lcd.printf("Time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  } else {
    M5.Lcd.println("NTP Sync Failed");
  }

  delay(2000);
}

void loop() {
  if (Serial2.available()) {
    M5.Lcd.fillScreen(BLACK);

    String rxs = Serial2.readStringUntil('\n'); // Read until newline
    Serial.print(rxs);
    M5.Lcd.setCursor(0, 8);

    // rxs: received ID
    String deviceID = rxs.substring(4);

    // Display on LCD
    M5.Lcd.println("Received ID: " + deviceID);

    // Get current time and display
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      char timeStr[20];
      sprintf(timeStr, "Time: %02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      M5.Lcd.println(timeStr);
    } else {
      M5.Lcd.println("Time: --:--:--");
    }

    String digitID = "LoRa_sender_" + deviceID;

    // Send to Google Sheets
    accessToGoogleSheets(digitID);

    yield();  // WDT
    delay(100);
  }
}
