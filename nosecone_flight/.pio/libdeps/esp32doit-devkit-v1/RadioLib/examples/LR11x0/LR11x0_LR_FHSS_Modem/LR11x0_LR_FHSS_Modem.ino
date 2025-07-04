/*
  RadioLib LR11x0 LR-FHSS Modem Example

  This example shows how to use LR-FHSS modem in LR11x0 chips.

  NOTE: The sketch below is just a guide on how to use
        LR-FHSS modem, so this code should not be run directly!
        Instead, modify the other examples to use LR-FHSS
        modem and use the appropriate configuration
        methods.

  For default module settings, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/Default-configuration#lr11x0---lr-fhss-modem

  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

// LR1110 has the following connections:
// NSS pin:   10
// IRQ pin:   2
// NRST pin:  3
// BUSY pin:  9
LR1110 radio = new Module(10, 2, 3, 9);

// or using RadioShield
// https://github.com/jgromes/RadioShield
//LR1110 radio = RadioShield.ModuleA;

void setup() {
  Serial.begin(9600);

  // initialize LR1110 with default settings
  Serial.print(F("[LR1110] Initializing ... "));
  int state = radio.beginLRFHSS();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // if needed, you can switch between any of the modems
  //
  // radio.begin()       start LoRa modem (and disable LR-FHSS)
  // radio.beginLRFHSS() start LR-FHSS modem (and disable LoRa)

  // the following settings can also
  // be modified at run-time
  state = radio.setFrequency(433.5);
  state = radio.setLrFhssConfig(RADIOLIB_LR11X0_LR_FHSS_BW_1523_4,  // bandwidth
                                RADIOLIB_LR11X0_LR_FHSS_CR_1_2,     // coding rate
                                3,                                  // header count
                                0x13A);                             // hopping sequence seed
  state = radio.setOutputPower(10.0);
  state = radio.setSyncWord(0x12345678);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print(F("Unable to set configuration, code "));
    Serial.println(state);
    while (true);
  }

  #warning "This sketch is just an API guide! Read the note at line 6."
}

void loop() {
  // LR-FHSS modem can use the same transmit/receive methods
  // as the LoRa modem, even their interrupt-driven versions

  // transmit LR-FHSS packet
  int state = radio.transmit("Hello World!");
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                      0x89, 0xAB, 0xCD, 0xEF};
    int state = radio.transmit(byteArr, 8);
  */
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("[LR1110] Packet transmitted successfully!"));
  } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
    Serial.println(F("[LR1110] Packet too long!"));
  } else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
    Serial.println(F("[LR1110] Timed out while transmitting!"));
  } else {
    Serial.println(F("[LR1110] Failed to transmit packet, code "));
    Serial.println(state);
  }

  // receive LR-FHSS packet
  String str;
  state = radio.receive(str);
  /*
    byte byteArr[8];
    int state = radio.receive(byteArr, 8);
  */
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("[LR1110] Received packet!"));
    Serial.print(F("[LR1110] Data:\t"));
    Serial.println(str);
  } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.println(F("[LR1110] Timed out while waiting for packet!"));
  } else {
    Serial.print(F("[LR1110] Failed to receive packet, code "));
    Serial.println(state);
  }

}
