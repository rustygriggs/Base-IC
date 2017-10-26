#include <Arduino.h>
#include <BaseIC.h>
#include <SoftwareSerial.h>

SoftwareSerial sSerial(8, 9); // RX, TX

#define VERBOSE false
#define OPEN 1
#define CLOSE 0

BaseIC baseIC = BaseIC(sSerial, VERBOSE);

/**
 * Register a hex, range, toggle, and read service.
 * Everthing needs to be characters. The zigbee module encoded and decodes
 * characters better than it does integers.
 */
uint8_t inputServices[1] = {'3'}; // Define one Toggle Input
uint8_t outputServices[1] = {'4'}; // Define one Toggle Output
uint8_t name[7] = {'C', 'u', 'r', 't', 'a', 'i', 'n'};
int position = 0;

void stepperGo(int timesToLoop, int direction)
{
    digitalWrite(6,LOW); // Set Enable low
    digitalWrite(10,LOW); // Set Enable low

    if(direction == OPEN)
    {
        digitalWrite(4,HIGH); // Set Dir high
        digitalWrite(11,LOW); // Set Dir high
    }
    else if(direction == CLOSE)
    {
        digitalWrite(4,LOW); // Set Dir high
        digitalWrite(11,HIGH); // Set Dir high
    }

    // Serial.println("Loop 200 steps (1 rev)");
    for(int x = 0; x < timesToLoop; x++) // Loop 200 times
    {
            digitalWrite(5,HIGH); // Output high
            digitalWrite(7,HIGH); // Output high
            delayMicroseconds(1500); // Wait
            digitalWrite(5,LOW); // Output low
            digitalWrite(7,LOW); // Output low
            delayMicroseconds(1500); // Wait
    }

        digitalWrite(6,HIGH); // Set Enable high
        digitalWrite(10,HIGH); // Set Enable high
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

    // If the response is for the toggle service and is service number 1.
    // THEN TOGGLE!!!
    if (serviceId == 4 && serviceNumber == 1) {
        Serial.print("Value: ");
        Serial.println(buffer);


        if (strcmp(buffer, "0") == 0) {
            // Close
            Serial.println("Close");
            if(position == 1) // curtain is half way open
            {
                stepperGo(400, CLOSE);
                position = 0;
            }
            else if(position == 2) // curtain is all the way open
            {
                stepperGo(800, CLOSE);
                position = 0;
            }
        }

        if (strcmp(buffer, "50") == 0) {
            // Open 50%
            Serial.println("Open 50%");
            if(position == 0)
            {
                stepperGo(400, OPEN); // curtain closed
                position = 1;
            }
            else if(position == 2)
            {
                stepperGo(400, CLOSE); // curtain all the way open
                position = 1;
            }
        }

        if (strcmp(buffer, "100") == 0) {
            // Open all the way
            Serial.println("Open all the way");
            if(position == 0) // curtain is closed
            {
                stepperGo(800, OPEN);
                position = 2;
            }
            else if(position == 1) // curtain is half way open
            {
                stepperGo(400, OPEN);
                position = 2;
            }
        }
    }
}

void setup() {
    pinMode(13, OUTPUT);
    pinMode(6,OUTPUT); // Enable
    pinMode(5,OUTPUT); // Step
    pinMode(4,OUTPUT); // Dir
    pinMode(2, INPUT_PULLUP); // But
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
    if(digitalRead(2) == LOW)
    {
        Serial.println("Button");
        digitalWrite(6,LOW); // Set Enable low
        digitalWrite(4,HIGH); // Set Dir high
        // Serial.println("Loop 200 steps (1 rev)");
        for(int x = 0; x < 200; x++) // Loop 200 times
        {
            digitalWrite(5,HIGH); // Output high
            delayMicroseconds(1500); // Wait
            digitalWrite(5,LOW); // Output low
            delayMicroseconds(1500); // Wait
        }
        Serial.println("Pause");
    } else {
        digitalWrite(6,HIGH); // Set Enable high
    }
    // Continuously let xbee read packets and call callbacks.
    baseIC.loop();
}
