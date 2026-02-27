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
#define RX_pin 26    // LoRa HAT
#define TX_pin 0     // LoRa HAT
#define RESET_pin -1 // No RESET Pin
#define BOOT_pin -1  // No Boot Pin

#include <M5Unified.h>
#include <Arduino.h>
#include "LoRa.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

const char* ssid = "JT-1F418A";        // ★必要に応じて変更
const char* password = "20679092";     // ★必要に応じて変更

const char* KEY = "AKfycbytUK_rnH8RacRQG14vvpJwBYoT0AE3jv4MLq9-TMpIx2fOuuKAdM0DOEHp_TSssv9h";

void accessToGoogleSheets(String latStr, String lngStr) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected.");
    return;
  }

  HTTPClient http;
  String url = "https://script.google.com/macros/s/";
  url += KEY;
  url += "/exec?";
  url += "lat=" + latStr;
  url += "&lng=" + lngStr;

  http.begin(url);
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
  M5.Lcd.setRotation(ROT);
  M5.Lcd.setTextSize(TSIZE);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.println("起動しました");
  delay(1000);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.println("LoRa");
  M5.Lcd.println("Display");
  M5.Lcd.println("Stand-by");

  // LoRa初期化
  LoRaInit(RX_pin, TX_pin, RESET_pin, BOOT_pin);

  // Wi-Fi接続
  WiFi.disconnect(true, true);  // 接続情報をクリア
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
    M5.Lcd.printf("RSSI: %d dBm\n", WiFi.RSSI());
  } else {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.println("WiFi NOT connected!");
    M5.Lcd.setTextColor(WHITE);
  }

  // NTP（時刻同期）
  configTime(9 * 3600, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp", "pool.ntp.org");
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    M5.Lcd.println("NTP Error!");
  } else {
    M5.Lcd.printf("Time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  }

  delay(2000);
}

void loop() {
  if (Serial2.available()) {
    M5.Lcd.fillScreen(BLACK);
    String rxs = Serial2.readStringUntil('\n');
    rxs.trim();
    Serial.print(rxs);
    M5.Lcd.setCursor(0, 0);

    int idx1 = rxs.indexOf('|');

    if (idx1 > 0) {
    String latStr = rxs.substring(4, idx1);
    String lngStr = rxs.substring(idx1 + 1);

    // LCD表示（floatに変換して表示する）
    float lat = latStr.toFloat();
    float lng = lngStr.toFloat();

    M5.Lcd.printf("lat: %.6f\n", lat);
    M5.Lcd.printf("lng: %.6f\n", lng);

    // 時刻表示省略

    // 文字列のままGoogle Sheets送信
    accessToGoogleSheets(latStr, lngStr);

    } else {
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.println("データ形式エラー");
    }

    yield();     // WDT対策
    delay(100);  // 軽めの待機
  }
}
