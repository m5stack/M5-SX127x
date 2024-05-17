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

void onCadDone(boolean signalDetected);
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

    // register the channel activity dectection callback
    LoRa.onCadDone(onCadDone);
    // register the receive callback
    LoRa.onReceive(onReceive);
    // put the radio into CAD mode
    LoRa.channelActivityDetection();
}

void loop() {
    // do nothing
}

void onCadDone(boolean signalDetected) {
    // detect preamble
    if (signalDetected) {
        Serial.println("Signal detected");
        // put the radio into continuous receive mode
        LoRa.receive();
    } else {
        // try next activity dectection
        LoRa.channelActivityDetection();
    }
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

    // put the radio into CAD mode
    LoRa.channelActivityDetection();
}
