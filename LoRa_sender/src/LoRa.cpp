//
//  920MHz LoRa/FSK RF module ES920LR3 
//
//  MaiaR create
//


#include "LoRa.h"

int rxPin;
int txPin;
int resetPin;
int bootPin;

void LoRaReset(){
  if (bootPin > 0){
    pinMode(bootPin, OUTPUT);   // BOOT "L"
    digitalWrite(bootPin, LOW);
    pinMode(resetPin, OUTPUT);  // NRST "L"
    digitalWrite(resetPin, LOW);
    delay(10);
    pinMode(resetPin, INPUT);   // NRST open
  } else { 
    pinMode(rxPin, OUTPUT);
    pinMode(txPin, OUTPUT);
    digitalWrite(rxPin, LOW);
    digitalWrite(txPin, LOW);
    delay(1000);
    pinMode(rxPin, INPUT);
    pinMode(txPin, INPUT);
  }
}


void LoRaConfigMode(){
  Serial.begin(115200);
  while(1){
    LoRaReset();
    Serial2.begin(115200, SERIAL_8N1, rxPin, txPin);
    Serial2.setTimeout(50);
    delay(50);
    String rx = Serial2.readString();
    Serial.println(rx);
    if (rx.indexOf("Select Mode [") >= 0) break;
    LoRaCommand("config");
    delay(100); 
  }
}


int LoRaCommand(String s){
  Serial2.print(s + "\r\n");
  Serial2.flush();
  Serial.println(s);
  String rx = Serial2.readString();
  Serial.print(rx);
  return (rx.indexOf("OK"));
}


int LoRaInit(int rx, int tx, int reset, int boot){
  rxPin = rx;
  txPin = tx;
  resetPin = reset;
  bootPin = boot;
  LoRaConfigMode();
  if (LoRaCommand("2"     ) < 0) return(-1); // processor mode
  if (LoRaCommand("x"     ) < 0) return(-1); // default
  if (LoRaCommand("b 4"   ) < 0) return(-1); // bandwidth 125kHz
  if (LoRaCommand("c 12"  ) < 0) return(-1); // spread factor SF12
  if (LoRaCommand("d 3"   ) < 0) return(-1); // channel 3
  if (LoRaCommand("e 2345") < 0) return(-1); // PAN ID
  if (LoRaCommand("f 1"   ) < 0) return(-1); // own ID
  if (LoRaCommand("g ffff") < 0) return(-1); // broadcast
  if (LoRaCommand("p 1"   ) < 0) return(-1); // RSSI on

  // （長距離強化）
  /*
  if (LoRaCommand("m 8"   ) < 0) return(-1); // 出力最大13dBm
  if (LoRaCommand("k 1"   ) < 0) return(-1); // CRCチェックON
  if (LoRaCommand("a 1"   ) < 0) return(-1); // Coding rate 4/5
  if (LoRaCommand("l 0"   ) < 0) return(-1); // 省電力モードOFF
  */

  if (LoRaCommand("z"     ) < 0) return(-1); // start
  return(0);
}

