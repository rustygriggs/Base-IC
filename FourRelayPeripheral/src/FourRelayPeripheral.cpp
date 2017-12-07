#include <Arduino.h>
#include <BaseIC.h>
#include <SoftwareSerial.h>

SoftwareSerial sSerial(8, 9); // RX, TX

#define VERBOSE false
#define RELAY1_PIN 4
#define RELAY2_PIN 5
#define RELAY3_PIN 6
#define RELAY4_PIN 7

BaseIC baseIC = BaseIC(sSerial, VERBOSE);

/**
 * Register a hex, range, toggle, and read service.
 * Everthing needs to be characters. The zigbee module encoded and decodes
 * characters better than it does integers.
 */
uint8_t inputServices[4] = {'3', '3', '3', '3'}; // Define four Toggle Input
uint8_t outputServices[1] = {'1'}; // Define one Hex Output
uint8_t name[10] = {'F', 'o', 'u', 'r', ' ', 'R', 'e', 'l', 'a', 'y'};

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

    // if (VERBOSE) {
        Serial.print("Service ID:\t");
        Serial.println(serviceId);
        Serial.print("Service Number:\t");
        Serial.println(serviceNumber);
        Serial.print("Value:\t\t");
        Serial.println(buffer);
    // }

    // If the response is for the hex service and is service number 1,
    // then toggle the appropriate relay.
    if (serviceId == 1 && serviceNumber == 1) {
        // Toggle relay 1
        if (strcmp(buffer, "01") == 0) {
            if (digitalRead(RELAY1_PIN) == HIGH) {
                digitalWrite(RELAY1_PIN, LOW);
            } else {
                digitalWrite(RELAY1_PIN, HIGH);
            }
        }

        // Toggle relay 2
        if (strcmp(buffer, "02") == 0) {
            if (digitalRead(RELAY2_PIN) == HIGH) {
                digitalWrite(RELAY2_PIN, LOW);
            } else {
                digitalWrite(RELAY2_PIN, HIGH);
            }
        }

        // Toggle relay 3
        if (strcmp(buffer, "03") == 0) {
            if (digitalRead(RELAY3_PIN) == HIGH) {
                digitalWrite(RELAY3_PIN, LOW);
            } else {
                digitalWrite(RELAY3_PIN, HIGH);
            }
        }

        // Toggle relay 4
        if (strcmp(buffer, "04") == 0) {
            if (digitalRead(RELAY4_PIN) == HIGH) {
                digitalWrite(RELAY4_PIN, LOW);
            } else {
                digitalWrite(RELAY4_PIN, HIGH);
            }
        }
    }
}

void setup() {
    pinMode(13, OUTPUT);

    pinMode(RELAY1_PIN, OUTPUT);
    pinMode(RELAY2_PIN, OUTPUT);
    pinMode(RELAY3_PIN, OUTPUT);
    pinMode(RELAY4_PIN, OUTPUT);

    digitalWrite(RELAY1_PIN, HIGH);
    digitalWrite(RELAY2_PIN, HIGH);
    digitalWrite(RELAY3_PIN, HIGH);
    digitalWrite(RELAY4_PIN, HIGH);

    Serial.begin(9600);

    baseIC.begin();

    baseIC.setNetworkID();

    baseIC.registerModule(
        name, sizeof(name),
        inputServices, sizeof(inputServices),
        outputServices, sizeof(outputServices));

    baseIC.attachListener(responseListener);
}

void loop() {
    // Continuously let xbee read packets and call callbacks.
    baseIC.loop();
}
