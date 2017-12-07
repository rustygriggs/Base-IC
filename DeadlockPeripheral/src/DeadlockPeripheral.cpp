#include <Arduino.h>
#include <BaseIC.h>
#include <SoftwareSerial.h>
#define VERBOSE false

SoftwareSerial sSerial(8, 9); // RX, TX

BaseIC baseIC = BaseIC(sSerial, VERBOSE);

/**
 * Register a hex, range, toggle, and read service.
 * Everthing needs to be characters. The zigbee module encoded and decodes
 * characters better than it does integers.
 */
uint8_t inputServices[2] = {'3', '3'}; // Define one Toggle Input
uint8_t outputServices[2] = {'3', '3'}; // Define one Toggle Output
uint8_t name[4] = {'D', 'o', 'o', 'r'};
const int doorEn = 3;
const int doorDirA = 4;
const int doorDirB = 5;
const int doorOpenDelay = 4500; //will need to be changed
const int doorCloseDelay = 6000;
const int enable = 7; //"D" on the driver board
const int dir = 10; // "P" on the driver board
const int motorDelay = 350;
const int buttonPin = 2;
const int ledPin = 13;      // the number of the LED pin
int isLocked = 0;
int isOpen = 0;

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers


void open()
{
    if (!isLocked) {
        Serial.println("in open function");
        digitalWrite(dir, HIGH); //open direction
        Serial.println("motor about to be turned on");
        digitalWrite(enable, LOW); //turn motor on
        delay(motorDelay); //wait one second
        Serial.println("motor about to be turned off");
        digitalWrite(enable, HIGH); //turn motor off.
        Serial.println("leaving open function");
        isLocked = 1;
    }
}

void close()
{
    if (isLocked) {
        Serial.println("in close function");
        digitalWrite(dir, LOW); //close direction
        Serial.println("motor about to be turned on");
        digitalWrite(enable, HIGH); //turn motor on
        delay(motorDelay); //wait one second
        Serial.println("motor about to be turned off");
        digitalWrite(enable, LOW); //turn motor off.
        Serial.println("leaving close function");
        isLocked = 0;
    }
}

void toggle() {
    if (!isLocked) {
        digitalWrite(13, LOW);
        open();
    } else {
        digitalWrite(13, HIGH);
        close();
    }
}

void openDoor() {
    if (!isOpen) {
        Serial.println("open door");
        digitalWrite(doorDirA, HIGH);
        digitalWrite(doorDirB, LOW);
        digitalWrite(doorEn, HIGH);
        delay(doorOpenDelay);
        digitalWrite(doorEn, LOW);
        isOpen = 1;
    }
}

void closeDoor() {
    if (isOpen) {
        // todo: close door
        Serial.println("close door");
        digitalWrite(doorDirA, LOW);
        digitalWrite(doorDirB, HIGH);
        digitalWrite(doorEn, HIGH);
        delay(doorCloseDelay);
        digitalWrite(doorEn, LOW);
        isOpen = 0;
    }
}

void toggleDoor() {
    if (!isOpen) {
        openDoor();
    }
    else {
        closeDoor();
    }
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
    if (serviceId == 3 && serviceNumber == 1) {
        if (strcmp(buffer, "01") == 0) {
            Serial.println("open deadlock");
            open();
        }
        if (strcmp(buffer, "02") == 0) {
            Serial.println("close deadlock");
            close();
        }
        if (strcmp(buffer, "03") == 0) {
            Serial.println("toggle deadlock");
            toggle();
        }
    }
    if (serviceId == 3 && serviceNumber == 2) {
        if (strcmp(buffer, "01") == 0) {
            Serial.println("open door");
            openDoor();
        }
        if (strcmp(buffer, "02") == 0) {
            Serial.println("close door");
            closeDoor();
        }
        if (strcmp(buffer, "03") == 0) {
            Serial.println("toggle door");
            toggleDoor();
        }
    }
}



void setup()
{
  // pinMode(13, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(doorDirA, OUTPUT);
  pinMode(doorDirB, OUTPUT);
  pinMode(doorEn, OUTPUT);
  // set initial LED state
  digitalWrite(ledPin, ledState);
  digitalWrite(enable, LOW); //initialize

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
  // Continuously let xbee read packets and call callbacks.
  baseIC.loop();

  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
      // reset the debouncing timer
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
      // than the debounce delay, so take it as the actual current state:

      // if the button state has changed:
      if (reading != buttonState) {
        buttonState = reading;

        // only toggle the LED if the new button state is HIGH
        if (buttonState == HIGH) {
          ledState = !ledState;
          if (ledState == HIGH) {
           Serial.println("before open function");
           open();
          }
          else {
           Serial.println("before close function");
           close();
          }
        }
      }
    }

    // set the LED:
    digitalWrite(ledPin, ledState);


    // save the reading.  Next time through the loop,
    // it'll be the lastButtonState:
    lastButtonState = reading;
}
