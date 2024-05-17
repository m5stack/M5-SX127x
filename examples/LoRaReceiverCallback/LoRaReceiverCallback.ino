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
// #define LORA_FREQ     433E6
#define LORA_FREQ     868E6
#define LORA_SF       12
#define LORA_BW       125E3
#define LORA_TX_POWER 17

void onReceive(int packetSize);

void setup() {
    Serial.begin(115200);

    Serial.println("LoRa Receiver Callback");
    SPI.begin(LORA_SCLK, LORA_MISO, LORA_MOSI, -1);  // SCK, MISO, MOSI, SS
    LoRa.setSPI(&SPI);
    LoRa.setPins(CS_PIN, RST_PIN, IRQ_PIN);  // set CS, reset, IRQ pin
    while (!LoRa.begin(LORA_FREQ)) {
        Serial.println("LoRa init fail.");
        delay(1000);
    }

    LoRa.setTxPower(LORA_TX_POWER);
    LoRa.setSignalBandwidth(LORA_BW);
    LoRa.setSpreadingFactor(LORA_SF);

    // Uncomment the next line to disable the default AGC and set LNA gain,
    // values between 1 - 6 are supported LoRa.setGain(6);

    // register the receive callback
    LoRa.onReceive(onReceive);

    // put the radio into receive mode
    LoRa.receive();
}

void loop() {
    // do nothing
}

void onReceive(int packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    for (int i = 0; i < packetSize; i++) {
        Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
}
