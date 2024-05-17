/*
  LoRa Duplex communication wth callback

  Sends a message every half second, and uses callback
  for new incoming messages. Implements a one-byte addressing scheme,
  with 0xFF as the broadcast address.

  Note: while sending, LoRa radio is not listening for incoming messages.
  Note2: when using the callback method, you can't use any of the Stream
  functions that rely on the timeout, such as readString, parseInt(), etc.

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
// #define LORA_FREQ     433E6
#define LORA_FREQ     868E6
#define LORA_SF       12
#define LORA_BW       125E3
#define LORA_TX_POWER 17

String outgoing;           // outgoing message
byte msgCount     = 0;     // count of outgoing messages
byte localAddress = 0xBB;  // address of this device
byte destination  = 0xFF;  // destination to send to
long lastSendTime = 0;     // last send time
int interval      = 2000;  // interval between sends

void sendMessage(String outgoing);
void onReceive(int packetSize);

void setup() {
    Serial.begin(115200);  // initialize serial

    Serial.println("LoRa Duplex with callback");
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

    LoRa.onReceive(onReceive);
    LoRa.receive();
    Serial.println("LoRa init succeeded.");
}

void loop() {
    if (millis() - lastSendTime > interval) {
        String message = "HeLoRa World!";  // send a message
        sendMessage(message);
        Serial.println("Sending " + message);
        lastSendTime = millis();             // timestamp the message
        interval     = random(2000) + 1000;  // 2-3 seconds
        LoRa.receive();                      // go back into receive mode
    }
}

void sendMessage(String outgoing) {
    LoRa.beginPacket();             // start packet
    LoRa.write(destination);        // add destination address
    LoRa.write(localAddress);       // add sender address
    LoRa.write(msgCount);           // add message ID
    LoRa.write(outgoing.length());  // add payload length
    LoRa.print(outgoing);           // add payload
    LoRa.endPacket();               // finish packet and send it
    msgCount++;                     // increment message ID
}

void onReceive(int packetSize) {
    if (packetSize == 0) return;  // if there's no packet, return

    // read packet header bytes:
    int recipient       = LoRa.read();  // recipient address
    byte sender         = LoRa.read();  // sender address
    byte incomingMsgId  = LoRa.read();  // incoming msg ID
    byte incomingLength = LoRa.read();  // incoming msg length

    String incoming = "";  // payload of packet

    while (LoRa.available()) {  // can't use readString() in callback, so
        incoming += (char)LoRa.read();  // add bytes one by one
    }

    if (incomingLength != incoming.length()) {  // check length for error
        Serial.println("error: message length does not match length");
        return;  // skip rest of function
    }

    // if the recipient isn't this device or broadcast,
    if (recipient != localAddress && recipient != 0xFF) {
        Serial.println("This message is not for me.");
        return;  // skip rest of function
    }

    // if message is for this device, or broadcast, print details:
    Serial.println("Received from: 0x" + String(sender, HEX));
    Serial.println("Sent to: 0x" + String(recipient, HEX));
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("Message length: " + String(incomingLength));
    Serial.println("Message: " + incoming);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println();
}
