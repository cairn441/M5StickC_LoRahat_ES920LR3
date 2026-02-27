/********************************************************************************
*
* LoRa    : ES920LR3
* sensor  : ENV III / ENV IV
* library : M5Unit-ENV 1.1.1
*
********************************************************************************/

#define ENV_VIA_GROVE
#define ENVIV
#define M5StickCPlus2 // M5StickC Plus2   with LoRa HAT and ENV UNIT / LoRa UNIT and ENV HAT

// * M5StickC Plus / Plus2 with LoRa UNIT and ENVIII HAT / LoRa HAT and ENVIII UNIT
#define ROT 3
#define TSIZE 2
#define RPOS 62
#define SX 135
#define SY 35
#define SR 8
// ENV III Unit via Grove and LoRa HAT
#define RX_pin 26     // LoRa HAT
#define TX_pin 0      // LoRa HAT
#define RESET_pin -1  // No RESET Pin
#define BOOT_pin -1   // No Boot Pin

#include <Arduino.h>
#include <M5Unified.h>
#include "LoRa.h"
#include <TinyGPSPlus.h>

const unsigned long interval = 10000;  // 10秒に一回LoRa送信
unsigned long prevMillis = 0;

TinyGPSPlus gps;
HardwareSerial GPSserial(1);  // UART1 使用

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.begin();
  M5.Display.setRotation(3);
  M5.Display.setTextSize(2);

  Serial.begin(115200);  // デバッグ用（PCと接続）

  LoRaInit(RX_pin, TX_pin, RESET_pin, BOOT_pin);
  
  // AT6668 の UART 通信設定（115200bps, RX = GPIO33）
  GPSserial.begin(115200, SERIAL_8N1, 33, -1);

  M5.Display.println("GPS Unit Ready...");
}


void loop() {
  while (GPSserial.available()) {
    gps.encode(GPSserial.read());
  }

  if (millis() - prevMillis >= interval) {
    prevMillis = millis();

    M5.Display.setCursor(0, 20);
    M5.Display.fillRect(0, 20, 135, 80, BLACK);

    if (gps.location.isUpdated()) {
      float lat = gps.location.lat();
      float lng = gps.location.lng();

      M5.Display.printf("Lat: %.6f\n", lat);
      M5.Display.printf("Lng: %.6f\n", lng);
      Serial.printf("Lat: %.6f, Lng: %.6f\n", lat, lng);

      String Lat = String(lat, 6);
      String Lng = String(lng, 6);
      String sendData = Lat + "|" + Lng;
      
      Serial2.print(sendData + "\r\n");  
      Serial.println("送信データ: " + sendData);
    } else {
      M5.Display.println("Waiting for GPS...");
    }
  }
}