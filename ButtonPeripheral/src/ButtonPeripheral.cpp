#include <Arduino.h>
#include <BaseIC.h>
#include <SoftwareSerial.h>

// debounce time; increase if necessary.
#define DEBOUNCE_DELAY 250
#define VERBOSE false

SoftwareSerial sSerial(8, 9); // RX, TX
BaseIC baseIC = BaseIC(sSerial, VERBOSE);

/**
 * Register a hex, range, toggle, and read service.
 * Everthing needs to be characters. The zigbee module encoded and decodes
 * characters better than it does integers.
 */
uint8_t inputServices[1] = {'1'}; // Define one HEX Input
uint8_t outputServices[1] = {'3'}; // Define one toggle output for the LED
uint8_t name[9] = {'1', '2', ' ', 'B', 'u', 't', 't', 'o', 'n'};

// Keep track of the last time that any button was pressed.
unsigned long buttonOneLastPressed = 0;
unsigned long buttonTwoLastPressed = 0;
unsigned long buttonThreeLastPressed = 0;

// The current values of the buttons.
int valueOne = 0;
int valueTwo = 0;
int valueThree = 0;

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

    Serial.print("Service ID:\t");
    Serial.println(serviceId);
    Serial.print("Service Number:\t");
    Serial.println(serviceNumber);
    Serial.print("Value:\t\t");
    Serial.println(buffer);

    // If the response is for the toggle service and is service number 1.
    // THEN TOGGLE!!!
    if (serviceId == 3 && serviceNumber == 1) {
        if (digitalRead(13) == HIGH) {
            digitalWrite(13, LOW);
        } else {
            digitalWrite(13, HIGH);
        }
    }
}

void setup() {
    pinMode(13, OUTPUT);
    pinMode(A0, INPUT_PULLUP);
    pinMode(A1, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);

    Serial.begin(9600);

    baseIC.begin();

    baseIC.setNetworkID();

    baseIC.registerModule(name, sizeof(name), inputServices,
                          sizeof(inputServices), outputServices,
                          sizeof(outputServices));

    baseIC.attachListener(responseListener);
}

void loop() {
    // Get the updated value :
    valueOne = digitalRead(A0);
    valueTwo = digitalRead(A1);
    valueThree = digitalRead(A2);

    if (valueOne == LOW && ((millis() - buttonOneLastPressed) > DEBOUNCE_DELAY)) {
        buttonOneLastPressed = millis();
        if (VERBOSE) {
            Serial.println("Button one pressed");
        }
        baseIC.sendInt8(1, 1); // serviceNumber, hexValue
    }

    if (valueTwo == LOW && ((millis() - buttonTwoLastPressed) > DEBOUNCE_DELAY)) {
        buttonTwoLastPressed = millis();
        if (VERBOSE) {
            Serial.println("Button two pressed");
        }
        baseIC.sendInt8(1, 2); // serviceNumber, hexValue
    }

    if (valueThree == LOW && ((millis() - buttonThreeLastPressed) > DEBOUNCE_DELAY)) {
        buttonThreeLastPressed = millis();
        if (VERBOSE) {
            Serial.println("Button three pressed");
        }
        baseIC.sendInt8(1, 3); // serviceNumber, hexValue
    }

    // Continuously let xbee read packets and call callbacks.
    baseIC.loop();
}
