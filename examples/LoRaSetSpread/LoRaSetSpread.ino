/*
  LoRa Duplex communication with Spreading Factor

  Sends a message every half second, and polls continually
  for new incoming messages. Sets the LoRa radio's spreading factor.

  Spreading factor affects how far apart the radio's transmissions
  are, across the available bandwidth. Radios with different spreading
  factors will not receive each other's transmissions. This is one way you
  can filter out radios you want to ignore, without making an addressing scheme.

  Spreading factor affects reliability of transmission at high rates, however,
  so avoid a huge spreading factor when you're sending continually.

  See the Semtech datasheet, http://www.semtech.com/images/datasheet/sx1276.pdf
  for more on Spreading Factor.

  created 28 April 2017
  by Tom Igoe
*/
#include <SPI.h>
#include "M5_SX127X.h"

// Module Connect Pins Config
#define CS_PIN  5
#define RST_PIN 13
#define IRQ_PIN 34

#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_SCLK 18

// LoRa Parameters Config
#define LORA_FREQ     868E6
#define LORA_SF       12
#define LORA_BW       125E3
#define LORA_TX_POWER 17

byte msgCount     = 0;     // count of outgoing messages
int interval      = 2000;  // interval between sends
long lastSendTime = 0;     // time of last packet send

void sendMessage(String outgoing);
void onReceive(int packetSize);

void setup() {
    Serial.begin(115200);  // initialize serial

    Serial.println("LoRa Duplex - Set spreading factor");
    SPI.begin(LORA_SCLK, LORA_MISO, LORA_MOSI, -1);  // SCK, MISO, MOSI, SS
    LoRa.setSPI(&SPI);
    // override the default CS, reset, and IRQ pins (optional)
    LoRa.setPins(CS_PIN, RST_PIN, IRQ_PIN);  // set CS, reset, IRQ pin
    while (!LoRa.begin(LORA_FREQ)) {
        Serial.println("LoRa init fail.");
        delay(1000);
    }

    LoRa.setTxPower(LORA_TX_POWER);
    LoRa.setSignalBandwidth(LORA_BW);
    LoRa.setSpreadingFactor(LORA_SF);
    Serial.println("LoRa init succeeded.");
}

void loop() {
    if (millis() - lastSendTime > interval) {
        String message = "HeLoRa World! ";  // send a message
        message += msgCount;
        sendMessage(message);
        Serial.println("Sending " + message);
        lastSendTime = millis();             // timestamp the message
        interval     = random(2000) + 1000;  // 2-3 seconds
        msgCount++;
    }

    // parse for a packet, and call onReceive with the result:
    onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing) {
    LoRa.beginPacket();    // start packet
    LoRa.print(outgoing);  // add payload
    LoRa.endPacket();      // finish packet and send it
    msgCount++;            // increment message ID
}

void onReceive(int packetSize) {
    if (packetSize == 0) return;  // if there's no packet, return

    // read packet header bytes:
    String incoming = "";

    while (LoRa.available()) {
        incoming += (char)LoRa.read();
    }

    Serial.println("Message: " + incoming);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println();
}
