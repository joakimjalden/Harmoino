// Minimal sketch to query a Harmony Smart Hub for the unique remote RF24 adress
// Run script with serial monitor (9600 baud), power on the hub and press the pair/reset button

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// nRF24L01+ SPI parameters
#define CE_PIN   9
#define CSN_PIN 10

// Harmony RF24 network and radio parameters
const uint64_t address = 0xBB0ADCA575; // Common pairing RF24 address
const uint8_t channels[12] = {5,8,14,17,32,35,41,44,62,65,71,74}; // Possible RF24 channels
int channelId = 0;

// Messages to send to Harmony Hub to get regular remote RF24 address
const byte pairMessage[22] = {242,95,1,225,154,157,218,83,40,64,30,4,2,7,12,0,0,0,0,0,102,100};
const byte pingMessage[5] = {242,64,1,225,236};
int pingRetries = 0;

RF24 radio(CE_PIN, CSN_PIN);


void setup() {

    Serial.begin(9600);

    SPI.begin();
    if(!radio.begin(&SPI)) {
      Serial.println("nRF24L01+ radio hardware not responding.");
      while(1); // Stop execution
    } else {
      Serial.println("nRF24L01+ radio hardware started.");
    }

    // Radio setup
    radio.setDataRate(RF24_2MBPS);
    radio.enableDynamicPayloads();
    radio.enableAckPayload();
    radio.setCRCLength (RF24_CRC_16);
    radio.openWritingPipe(address);

    Serial.println("Power on the Harmony Smart Hub and press the pair/reset button on the hub!");
    
}

void loop() {

    // Send out data to trigger the Hub
    if(pingRetries == 0) {
        radio.setChannel(channels[channelId]);
        if(radio.write(&pairMessage, sizeof(pairMessage))) {
            pingRetries = 10;
        } else {
            channelId++;
            if(channelId > 11) channelId = 0;
        }
    } else {
        radio.write(&pingMessage, sizeof(pingMessage));
        pingRetries--;
    }

    delay(100);

    // Look for and interpret ACK payloads
    if(radio.isAckPayloadAvailable()) {
        uint8_t dataReceived[32];
        int payloadSize = radio.getDynamicPayloadSize();
        radio.read(&dataReceived,payloadSize);

        if(payloadSize == 22) {
            Serial.print("The remote RF24 address is 0x");
            for(int i=3;i<8;i++) {
                char tmp[3];
                if(i < 7) sprintf(tmp,"%.2X",dataReceived[i]);
                else sprintf(tmp,"%.2X",dataReceived[i]-1);
                Serial.print(tmp);
            }
             Serial.println("");
             Serial.println("Script halted");
             while(1);
        }      
    }
    
}
