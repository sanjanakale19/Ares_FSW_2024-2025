#include <ESP32-TWAI-CAN.hpp>
#include "CANTX.h"
#include "CANRX.h"

void setup() {
  Serial.begin(115200);
  CANTX::setupCAN();
  // CANRX::setupCAN();
}

void loop() {
  String s = "hello!";
  // Serial.println("length of tx str = " + String(s.length()));
  CANTX::encodeMessage(s);
  // CANRX::decodeMessage();

  delay(100);
}