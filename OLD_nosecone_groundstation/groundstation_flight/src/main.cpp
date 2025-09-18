#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>

// vspi
const uint8_t SCK_PIN = 37;
const uint8_t MISO_PIN = 38;
const uint8_t MOSI_PIN = 36;
SPIClass* VSPI_bus;

// radio spi
const uint32_t CS_PIN = 13;
const uint32_t RST = 35;
const uint32_t D0 = 10;
const uint8_t D1 = 11;

// tx and rx enable
const uint8_t tx_en = 5;
const uint8_t rx_en = 6;

// radio parameters
#define syncWord 0x45
#define transmissionFreqA 915E6

LoRaClass radioA;

void setup() {
  Serial.begin(9600);

  VSPI_bus = new SPIClass(HSPI);
  VSPI_bus->begin(SCK_PIN, MISO_PIN, MOSI_PIN);

  Serial.println("LoRa Receiver");

  radioA.setPins(CS_PIN, RST, D0);
  radioA.setSPI(VSPI_bus);

  // for ground station 2024-2025, tx_en = 5, rx_en = 6
  radioA.setBoostPins(tx_en, rx_en);
  
  if (!radioA.begin(transmissionFreqA)) {
    Serial.println("Starting LoRa A failed!");
  }

  radioA.setSyncWord(syncWord);
  // radioB.setSyncWord(syncWord);
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

    digitalWrite(rx_en, HIGH);
    delay(100);
    digitalWrite(rx_en, LOW);
  }

}