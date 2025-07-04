#include <Arduino.h>
// #include <RadioLib.h>
#include <LoRa.h>
#include <SPI.h>

const uint8_t SCK_PIN_A = 37;
const uint8_t MISO_PIN_A = 38;
const uint8_t MOSI_PIN_A = 36;
SPIClass* SPI_A;

const uint32_t CS_PIN_A = 13;
const uint32_t RST_A = 35;
const uint32_t D0_A = 10;
const uint8_t D1_A = 11;

#define syncWord 0x45
#define transmissionFreqA 915E6

LoRaClass radioA;

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1278 radio_A= RadioShield.ModuleA;

void setup() {
  Serial.begin(9600);

  SPI_A = new SPIClass(HSPI);
  SPI_A->begin(SCK_PIN_A, MISO_PIN_A, MOSI_PIN_A);

  Serial.println("LoRa Receiver");

  radioA.setPins(CS_PIN_A, RST_A, D0_A);
  radioA.setSPI(SPI_A);
  radioA.setBoostPins(5, 6);
  
  if (!radioA.begin(transmissionFreqA)) {
    Serial.println("Starting LoRa A failed!");
  }

  radioA.setSyncWord(syncWord);
  Serial.println("LoRa Initializing OK!");

}

void loop() {


  // Radio A 
  int packetSizeA = radioA.parsePacket();
  if (packetSizeA) {
    // received a packet
    Serial.print("'");

    // read packet
    while (radioA.available()) {
      Serial.print((char)radioA.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.print(",");
    Serial.print(radioA.packetRssi());
    Serial.print(",");
    // print Freq Error of packet
    Serial.print(" with Error ");
    Serial.print(",");
    Serial.print(radioA.packetFrequencyError());
    Serial.print(",");
    // print SNR of packet
    Serial.print(" with SNR ");
    Serial.print(",");
    Serial.println(radioA.packetSnr());
  }

}