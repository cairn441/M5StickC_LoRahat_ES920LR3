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
#define SCL_pin 33    // ENV III via Grove
#define SDA_pin 32    // ENV III via Grove
#define RX_pin 26     // LoRa HAT
#define TX_pin 0      // LoRa HAT
#define RESET_pin -1  // No RESET Pin
#define BOOT_pin -1   // No Boot Pin


#include <Arduino.h>
#include <M5Unified.h>
#include "M5UnitENV.h"
#include "LoRa.h"


#ifdef ENVIV
  SHT4X sht;
  BMP280 xmp;
  #define SHT_I2C_ADDR SHT40_I2C_ADDR_44
  #define XMP_I2C_ADDR BMP280_I2C_ADDR
#endif


float tmp = 0.0;
float hum = 0.0;
float pressure = 0.0;

#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 60 * 60  //60分に変更中
uint64_t sleep_duration = TIME_TO_SLEEP * uS_TO_S_FACTOR;

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.begin();

  Wire.end();
  sht.begin(&Wire, SHT_I2C_ADDR, SDA_pin, SCL_pin, 400000U);
  xmp.begin(&Wire, XMP_I2C_ADDR, SDA_pin, SCL_pin, 400000U);
  LoRaInit(RX_pin, TX_pin, RESET_pin, BOOT_pin);

  M5.Lcd.setRotation(ROT);
  M5.Lcd.setTextSize(TSIZE);
  
  //以下、本来loopにしていた部分
  //LoRa送信
  String deviceID = "1"; //デバイスごとに変更が必要

  String sendData = deviceID;
  LoRaCommand(sendData);



  //LoRaCommand("MC"+String(tmp)+" "+String(hum)+((pressure < 1000)? "  ":" ")+String(pressure));

  delay(5000);

  M5.Display.sleep();

  
  esp_sleep_enable_timer_wakeup(sleep_duration);
  esp_deep_sleep_start();
}

void loop() {
}
