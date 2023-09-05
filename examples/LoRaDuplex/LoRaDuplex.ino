/**
 * @file LoRaDuplex.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief LoRa868 Module LoRaDuplex Demo.
 * @version 0.1
 * @date 2023-07-25
 *
 *
 * @Hardwares: M5Core1/2/3 LoRa868 Module
 * @Platform Version: Arduino M5Stack Board Manager v2.0.7
 * @Dependent Library:
 * M5Unified://github.com/m5stack/M5Unified
 * M5GFX: https://github.com/m5stack/M5GFX
 * LoRa: https://github.com/sandeepmistry/arduino-LoRa
 */

#include <M5Unified.h>
#include <LoRa.h>
#include <M5GFX.h>

#define M5CORE1_DEVICE
// #define M5CORE2_DEVICE
// #define M5CORE3_DEVICE

M5GFX display;
M5Canvas canvas(&display);

String outgoing;           // outgoing message
byte msgCount     = 0;     // count of outgoing messages
byte localAddress = 0xBB;  // address of this device
byte destination  = 0xFF;  // destination to send to
long lastSendTime = 0;     // last send time
int interval      = 2000;  // interval between sends

void onReceive(int packetSize);
void sendMessage(String outgoing);

void gfx_canvas_init() {
    display.begin();
    canvas.setColorDepth(1);  // mono color
    canvas.setFont(&fonts::efontCN_14);
    canvas.createSprite(display.width(), display.height());
    canvas.setTextSize(2);
    canvas.setPaletteColor(1, GREEN);
    canvas.pushSprite(0, 0);
}

char info_str[100] = {0};

void update_lora_config() {
    LoRa.setTxPower(17);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setSpreadingFactor(12);
    // LoRa.enableCrc();
}

void lora_init() {
#if defined M5CORE1_DEVICE
    LoRa.setPins(5, 26, 36);

#elif defined M5CORE2_DEVICE
    LoRa.setPins(33, 26, 36);

#elif defined M5CORE3_DEVICE
    LoRa.setPins(1, 9, 8);
#endif

    if (!LoRa.begin(
            868E6)) {  // initialize ratio at 868 MHz.  868 MHz 时的初始化比率
        Serial.println("LoRa init failed. Check your connections.");
        while (true)
            ;  // if failed, do nothing
    }
    update_lora_config();
}

void setup() {
    M5.begin();
    gfx_canvas_init();
    lora_init();
    Serial.println("LoRa init succeeded.");
}

void loop() {
    if (millis() - lastSendTime > interval) {
        String message =
            "HeLoRa World!: " + String(msgCount);  // send a message
        sendMessage(message);
        canvas.setTextSize(2);
        canvas.fillRect(0, 0, 320, 32, BLACK);
        canvas.drawString(message + String(">>"), 0, 0);
        canvas.pushSprite(0, 0);

        Serial.println("Sending " + message);
        lastSendTime = millis();             // timestamp the message
        interval     = random(2000) + 1000;  // 2-3 seconds
    }

    // parse for a packet, and call onReceive with the result:
    onReceive(LoRa.parsePacket());
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

    String incoming = "";

    while (LoRa.available()) {
        incoming += (char)LoRa.read();
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
    Serial.println("<<Received from: 0x" + String(sender, HEX));
    Serial.println("Sent to: 0x" + String(recipient, HEX));
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("Message length: " + String(incomingLength));
    Serial.println("Message: " + incoming);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println();
    canvas.fillRect(0, 32, 320, 205, BLACK);
    canvas.setCursor(0, 32);
    canvas.println("<<Received from: 0x" + String(sender, HEX));
    canvas.println("Sent to: 0x" + String(recipient, HEX));
    canvas.println("Msg ID: " + String(incomingMsgId));
    canvas.println("Msg length: " + String(incomingLength));
    canvas.println("Msg: " + incoming);
    canvas.println("RSSI: " + String(LoRa.packetRssi()));
    canvas.println("Snr: " + String(LoRa.packetSnr()));
    canvas.println();
    canvas.pushSprite(0, 0);
}
