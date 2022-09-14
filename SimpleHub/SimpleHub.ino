// Minimal sketch to receive RF24 commands from a Harmony Remote
// Prints packet data to the serial monitor (9600 baud)

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// nRF24L01+ SPI parameters
#define CE_PIN   9
#define CSN_PIN 10

// Harmony RF24 network and radio parameters
const uint64_t address = 0xFFFFFFFFFF; // Unique remote RF24 address. Use NetworkAddress script to find it
const uint8_t channel = 5; // Choose 5,8,14,17,32,35,41,44,62,65,71 or 74

RF24 radio(CE_PIN, CSN_PIN);

char dataReceived[32]; // this must match dataToSend in the TX (need more than 10?)
uint8_t payloadSize;
bool newData = false;


void setup() {

    Serial.begin(9600);

    SPI.begin();
    if(!radio.begin(&SPI)) {
      Serial.println("nRF24L01+ Radio hardware not responding.");
      while(1); // Stop execution
    } else {
      Serial.println("nRF24L01+ Radio hardware started.");
    }
    
    // Radio settings
    radio.setChannel(channel);
    radio.setDataRate(RF24_2MBPS);
    radio.enableDynamicPayloads();
    radio.setCRCLength (RF24_CRC_16);
    radio.openReadingPipe(1, address & 0xFFFFFFFF00);
    radio.openReadingPipe(2, address & 0xFFFFFFFFFF);
    radio.startListening();

    Serial.println("Press a button on the Harmony remote to get started!");
    
}

void loop() {

    uint8_t pipeNum;
    if ( radio.available(&pipeNum) ) {
        // Read packet
        uint8_t dataReceived[32];
        int payloadSize = radio.getDynamicPayloadSize();
        radio.read(&dataReceived,payloadSize);

        // Print contents
        Serial.print("0x");
        for(int i=0; i<payloadSize; i++) {
          char tmp[3];
          sprintf(tmp,"%.2X",dataReceived[i]);
          Serial.print(tmp);
          if(i<payloadSize-1) Serial.print(".");
        }
        Serial.print(" (");
        Serial.print(payloadSize);
        Serial.print(" bytes on pipe ");
        Serial.print(pipeNum);
        Serial.println(")");
           
    }
    
}
