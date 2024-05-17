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

int counter = 0;

void setup() {
    Serial.begin(115200);

    Serial.println("LoRa Sender non-blocking");
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
}

void loop() {
    // wait until the radio is ready to send a packet
    while (LoRa.beginPacket() == 0) {
        Serial.print("waiting for radio ... ");
        delay(100);
    }

    Serial.print("Sending packet non-blocking: ");
    Serial.println(counter);

    // send in async / non-blocking mode
    LoRa.beginPacket();
    LoRa.print("hello ");
    LoRa.print(counter);
    LoRa.endPacket(true);  // true = async / non-blocking mode

    counter++;
}
