#include <Arduino.h>
#include <BaseIC.h>
#include <SoftwareSerial.h>

// debounce time; increase if necessary.
#define DEBOUNCE_DELAY 250
#define VERBOSE false

SoftwareSerial sSerial(8, 9); // RX, TX
BaseIC baseIC = BaseIC(sSerial, VERBOSE);

#define BUTTON_ONE_PIN 4
#define BUTTON_TWO_PIN 3
#define BUTTON_THREE_PIN 2
#define BUTTON_FOUR_PIN 7
#define BUTTON_FIVE_PIN 6
#define BUTTON_SIX_PIN 5
#define BUTTON_SEVEN_PIN 12
#define BUTTON_EIGHT_PIN 11
#define BUTTON_NINE_PIN 10

/**
 * Register a hex, range, toggle, and read service.
 * Everthing needs to be characters. The zigbee module encoded and decodes
 * characters better than it does integers.
 */
uint8_t inputServices[9] = {'3', '3', '3', '3', '3', '3', '3', '3', '3'}; // Define nine toggle inputs
uint8_t outputServices[1] = {'3'}; // Define one toggle output for the LED
uint8_t name[8] = {'9', ' ', 'B', 'u', 't', 't', 'o', 'n'};

// Keep track of the last time that any button was pressed.
unsigned long buttonOneLastPressed = 0;
unsigned long buttonTwoLastPressed = 0;
unsigned long buttonThreeLastPressed = 0;
unsigned long buttonFourLastPressed = 0;
unsigned long buttonFiveLastPressed = 0;
unsigned long buttonSixLastPressed = 0;
unsigned long buttonSevenLastPressed = 0;
unsigned long buttonEightLastPressed = 0;
unsigned long buttonNineLastPressed = 0;

// The current values of the buttons.
int valueOne = 0;
int valueTwo = 0;
int valueThree = 0;
int valueFour = 0;
int valueFive = 0;
int valueSix = 0;
int valueSeven = 0;
int valueEight = 0;
int valueNine = 0;

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
    pinMode(BUTTON_ONE_PIN, INPUT_PULLUP);
    pinMode(BUTTON_TWO_PIN, INPUT_PULLUP);
    pinMode(BUTTON_THREE_PIN, INPUT_PULLUP);
    pinMode(BUTTON_FOUR_PIN, INPUT_PULLUP);
    pinMode(BUTTON_FIVE_PIN, INPUT_PULLUP);
    pinMode(BUTTON_SIX_PIN, INPUT_PULLUP);
    pinMode(BUTTON_SEVEN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_EIGHT_PIN, INPUT_PULLUP);
    pinMode(BUTTON_NINE_PIN, INPUT_PULLUP);

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
    valueOne = digitalRead(BUTTON_ONE_PIN);
    valueTwo = digitalRead(BUTTON_TWO_PIN);
    valueThree = digitalRead(BUTTON_THREE_PIN);
    valueFour = digitalRead(BUTTON_FOUR_PIN);
    valueFive = digitalRead(BUTTON_FIVE_PIN);
    valueSix = digitalRead(BUTTON_SIX_PIN);
    valueSeven = digitalRead(BUTTON_SEVEN_PIN);
    valueEight = digitalRead(BUTTON_EIGHT_PIN);
    valueNine = digitalRead(BUTTON_NINE_PIN);

    if (valueOne == LOW && ((millis() - buttonOneLastPressed) > DEBOUNCE_DELAY)) {
        buttonOneLastPressed = millis();
        if (VERBOSE) {
            Serial.println("Button one pressed");
        }
        baseIC.sendInt8(3, 1, 1); // serviceId, serviceNumber, value
    }

    if (valueTwo == LOW && ((millis() - buttonTwoLastPressed) > DEBOUNCE_DELAY)) {
        buttonTwoLastPressed = millis();
        if (VERBOSE) {
            Serial.println("Button two pressed");
        }
        baseIC.sendInt8(3, 2, 1); // serviceId, serviceNumber, value
    }

    if (valueThree == LOW && ((millis() - buttonThreeLastPressed) > DEBOUNCE_DELAY)) {
        buttonThreeLastPressed = millis();
        if (VERBOSE) {
            Serial.println("Button three pressed");
        }
        baseIC.sendInt8(3, 3, 1); // serviceId, serviceNumber, value
    }

    if (valueFour == LOW && ((millis() - buttonFourLastPressed) > DEBOUNCE_DELAY)) {
        buttonFourLastPressed = millis();
        if (VERBOSE) {
            Serial.println("Button four pressed");
        }
        baseIC.sendInt8(3, 4, 1); // serviceId, serviceNumber, value
    }

    if (valueFive == LOW && ((millis() - buttonFiveLastPressed) > DEBOUNCE_DELAY)) {
        buttonFiveLastPressed = millis();
        if (VERBOSE) {
            Serial.println("Button five pressed");
        }
        baseIC.sendInt8(3, 5, 1); // serviceId, serviceNumber, value
    }

    if (valueSix == LOW && ((millis() - buttonSixLastPressed) > DEBOUNCE_DELAY)) {
        buttonSixLastPressed = millis();
        if (VERBOSE) {
            Serial.println("Button six pressed");
        }
        baseIC.sendInt8(3, 6, 1); // serviceId, serviceNumber, value
    }

    if (valueSeven == LOW && ((millis() - buttonSevenLastPressed) > DEBOUNCE_DELAY)) {
        buttonSevenLastPressed = millis();
        if (VERBOSE) {
            Serial.println("Button seven pressed");
        }
        baseIC.sendInt8(3, 7, 1); // serviceId, serviceNumber, value
    }

    if (valueEight == LOW && ((millis() - buttonEightLastPressed) > DEBOUNCE_DELAY)) {
        buttonEightLastPressed = millis();
        if (VERBOSE) {
            Serial.println("Button eight pressed");
        }
        baseIC.sendInt8(3, 8, 1); // serviceId, serviceNumber, value
    }

    if (valueNine == LOW && ((millis() - buttonNineLastPressed) > DEBOUNCE_DELAY)) {
        buttonNineLastPressed = millis();
        if (VERBOSE) {
            Serial.println("Button nine pressed");
        }
        baseIC.sendInt8(3, 9, 1); // serviceId, serviceNumber, value
    }

    // Continuously let xbee read packets and call callbacks.
    baseIC.loop();
}
