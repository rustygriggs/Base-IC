#include <Arduino.h>
#include <BaseIC.h>
#include <SoftwareSerial.h>
#include <IRremote.h>

SoftwareSerial sSerial(8, 9); // RX, TX

#define VERBOSE true

BaseIC baseIC = BaseIC(sSerial, VERBOSE);

IRsend irsend;
/**
 * Register a hex, range, toggle, and read service.
 * Everthing needs to be characters. The zigbee module encoded and decodes
 * characters better than it does integers.
 */
uint8_t inputServices[9] = {'3', '3', '3', '3', '3', '3', '3', '3', '3'}; // Define twelve Toggle Inputs
uint8_t outputServices[1] = {'1'}; // Define one HEX Output
uint8_t name[9] = {'I', 'R', ' ', 'R', 'e', 'm', 'o', 't', 'e'};

#define DELAY 50
#define BUTTONPIN 12

// button states
int BS12;

//last button states
int LBS12 = LOW;

// last debounced times
unsigned long LDT12 = 0;

void debounceButton(int buttonRead, int* lastButtonState, unsigned long* lastDebounceTime, int* buttonState)
{
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonRead);
  // check to see if you just pressed the button
  // (i.e. the input went from LOWd to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != *lastButtonState) {
    // reset the debouncing timer
    *lastDebounceTime = millis();
  }

  if ((millis() - *lastDebounceTime) > DELAY) {

    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != *buttonState) {
      *buttonState = reading;

      if(*buttonState == LOW)
      {
        //irsend.sendSAMSUNG(0xE0E040BF, 32);
        //delay(5000);
        irsend.sendSAMSUNG(0xE0E020DF, 32);
        //delay(1500);
        //irsend.sendSAMSUNG(0xE0E0609F, 32);
      }
    }
  }
    *lastButtonState = reading;
}

void responseListener(ZBRxResponse &rx, uintptr_t) {
    uint8_t *data = rx.getFrameData() + rx.getDataOffset();
    char buffer[500];
    int dataPos = 0, bufferPos = 0, serviceId = 0, serviceNumber = 0;

    // Get Service ID by parsing the frame data until the first tab.
    for (dataPos = 0; ((dataPos < 500) && (dataPos < rx.getDataLength())); dataPos++) {
        if (data[dataPos] == '\t') {
            break;
        }

        buffer[bufferPos++] = data[dataPos];
    }

    // Then the data that is stored in the buffer can be converted to an int to represent the Service Id.
    dataPos++;
    buffer[bufferPos++] = '\0';
    serviceId = atoi(buffer);

    // Get Service Number by parsing the frame data until the first tab.
    bufferPos = 0;
    for (; ((dataPos < 500) && (dataPos < rx.getDataLength())); dataPos++) {
        if (data[dataPos] == '\t') {
            break;
        }

        buffer[bufferPos++] = data[dataPos];
    }

    // Then the data that is stored in the buffer can be converted to an int to represent the Service Number.
    dataPos++;
    buffer[bufferPos++] = '\0';
    serviceNumber = atoi(buffer);

    // Get Value by parsing the frame data until the new line.
    bufferPos = 0;
    for (; ((dataPos < 500) && (dataPos < rx.getDataLength())); dataPos++) {
        if (data[dataPos] == '\n') {
            break;
        }

        buffer[bufferPos++] = data[dataPos];
    }

    // We don't have to do anything with the buffer now. It will contain a char representation of the value.
    buffer[bufferPos++] = '\0';

    if (VERBOSE) {
        Serial.print("Service ID:\t");
        Serial.println(serviceId);
        Serial.print("Service Number:\t");
        Serial.println(serviceNumber);
        Serial.print("Value:\t\t");
        Serial.println(buffer);
    }

    // If the response is for the range service and is service number 1.
    // THEN TOGGLE!!!
    if (serviceId == 1 && serviceNumber == 1) {
        Serial.print("Value: ");
        Serial.println(buffer);
        if(strcmp(buffer, "01") == 0){
            irsend.sendSAMSUNG(0xE0E040BF, 32); // power
        }
        else if(strcmp(buffer, "02") == 0){
            irsend.sendSAMSUNG(0xE0E0E01F, 32); // volume up
        }
        else if(strcmp(buffer, "03") == 0){
            irsend.sendSAMSUNG(0xE0E0D02F, 32); // volume down
        }
        else if(strcmp(buffer, "04") == 0){
            irsend.sendSAMSUNG(0xE0E08877, 32); // 0
        }
        else if(strcmp(buffer, "05") == 0){
            irsend.sendSAMSUNG(0xE0E020DF, 32); // 1
        }
        else if(strcmp(buffer, "06") == 0){
            irsend.sendSAMSUNG(0xE0E0A05F, 32); // 2
        }
        else if(strcmp(buffer, "07") == 0){
            irsend.sendSAMSUNG(0xE0E0609F, 32); // 3
        }
        else if(strcmp(buffer, "08") == 0){
            irsend.sendSAMSUNG(0xE0E010EF, 32); // 4
        }
        else if(strcmp(buffer, "09") == 0){
            irsend.sendSAMSUNG(0xE0E0906F, 32); // 5
        }
        else if(strcmp(buffer, "10") == 0){
            irsend.sendSAMSUNG(0xE0E050AF, 32); // 6
        }
        else if(strcmp(buffer, "11") == 0){
            irsend.sendSAMSUNG(0xE0E030CF, 32); // 7
        }
        else if(strcmp(buffer, "12") == 0){
            irsend.sendSAMSUNG(0xE0E0B04F, 32); // 8
        }
        else if(strcmp(buffer, "13") == 0){
            irsend.sendSAMSUNG(0xE0E0708F, 32); // 9
        }
        else if(strcmp(buffer, "14") == 0){
            Serial.println("Preset 1");
            irsend.sendSAMSUNG(0xE0E040BF, 32);
            delay(7000);
            irsend.sendSAMSUNG(0xE0E020DF, 32); // preset 1
            delay(1000);
            irsend.sendSAMSUNG(0xE0E0609F, 32);
        }
        else if(strcmp(buffer, "15") == 0){
            Serial.println("Preset 2");
            irsend.sendSAMSUNG(0xE0E020DF, 32); // preset 1
            delay(1000);
            irsend.sendSAMSUNG(0xE0E010EF, 32);
        }
        else if(strcmp(buffer, "16") == 0){
            Serial.println("Preset 3");
            irsend.sendSAMSUNG(0xE0E020DF, 32); // preset 3
        }

    }
}

void setup() {

    pinMode(BUTTONPIN, INPUT);

    Serial.begin(9600);

    baseIC.begin();

    baseIC.setNetworkID();

    baseIC.registerModule(
        name, sizeof(name),
        inputServices, sizeof(inputServices),
        outputServices, sizeof(outputServices)
    );

    baseIC.attachListener(responseListener);
}

void loop() {
    //irsend.sendSAMSUNG(0xE0E020DF, 32);
    //delay(5000);
    // Continuously let xbee read packets and call callbacks.
    baseIC.loop();
}
