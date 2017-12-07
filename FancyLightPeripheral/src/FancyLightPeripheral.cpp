#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <BaseIC.h>
#include <SoftwareSerial.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

SoftwareSerial sSerial(8, 9); // RX, TX

#define LED_PIN 3
#define VERBOSE true

BaseIC baseIC = BaseIC(sSerial, VERBOSE);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, LED_PIN, NEO_GRB + NEO_KHZ800);

int mode = 0;
unsigned long lastMode0Time = 0;
unsigned long lastMode1Time = 0;
unsigned long lastMode2Time = 0;

uint16_t colorWipeIndex = 0;
int theaterChaseColor = 0;
int theaterChaseQ = 0;
uint16_t rainbowI = 0;
uint16_t rainbowJ = 0;

/**
 * Register a hex, range, toggle, and read service.
 * Everthing needs to be characters. The zigbee module encoded and decodes
 * characters better than it does integers.
 */
uint8_t inputServices[3] = {'3', '3', '3'}; // Define three Toggle Input
uint8_t outputServices[1] = {'1'}; // Define one Hex Output
uint8_t name[9] = {'F', 'a', 'n', 'c', 'y', ' ', 'L', 'E', 'D'};

char buffer[500];
int dataPos = 0, bufferPos = 0, serviceId = 0, serviceNumber = 0;

void responseListener(ZBRxResponse &rx, uintptr_t) {
    uint8_t *data = rx.getData();

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
    if (serviceId == 1 && serviceNumber == 1) {
        if (strcmp(buffer, "01") == 0) {
            Serial.println("Play pattern 1");
            mode = 0;
        }
        if (strcmp(buffer, "02") == 0) {
            Serial.println("Play pattern 2");
            mode = 1;
        }
        if (strcmp(buffer, "03") == 0) {
            Serial.println("Play pattern 3");
            mode = 2;
        }
    }
}

void setup() {
    pinMode(13, OUTPUT);
    Serial.begin(9600);

    baseIC.begin();

    baseIC.setNetworkID();

    baseIC.registerModule(
        name, sizeof(name),
        inputServices, sizeof(inputServices),
        outputServices, sizeof(outputServices));

    baseIC.attachListener(responseListener);

    strip.begin();
    strip.show(); // Initialize all pixels to 'off'

    lastMode0Time = millis();
    lastMode1Time = millis();
    lastMode2Time = millis();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c1, uint32_t c2, uint32_t c3) {
    uint32_t c = c1;
    uint16_t index = colorWipeIndex;
    if (colorWipeIndex >= strip.numPixels() && colorWipeIndex < strip.numPixels() * 2) {
        c = c2;
        index = colorWipeIndex - strip.numPixels();
    }
    if (colorWipeIndex >= strip.numPixels() * 2 && colorWipeIndex < strip.numPixels() * 3) {
        c = c3;
        index = colorWipeIndex - (strip.numPixels() * 2);
    }

    strip.setPixelColor(index, c);
    strip.show();

    if (++colorWipeIndex > strip.numPixels() * 3) {
        colorWipeIndex = 0;
    }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c1, uint32_t c2, uint32_t c3) {

    uint32_t c = c1;
    if (theaterChaseColor > 40 && theaterChaseColor <= 80) {
        c = c2;
    } else if (theaterChaseColor > 80 && theaterChaseColor <= 120) {
        c = c3;
    }

    for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + theaterChaseQ, c); //turn every third pixel on
    }

    strip.show();

    for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + theaterChaseQ, 0); //turn every third pixel off
    }

    if (++theaterChaseQ >= 3) {
        theaterChaseQ = 0;
    }

    if (++theaterChaseColor >= 120) {
        theaterChaseColor = 0;
    }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85) {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbow() {

    strip.setPixelColor(rainbowI, Wheel((rainbowI + rainbowJ) & 255));
    strip.show();

    if (++rainbowJ >= 256) {
        rainbowJ = 0;
    }

    if (++rainbowI >= strip.numPixels()) {
        rainbowI = 0;
    }
}

void loop() {
    if (mode == 0 && ((millis() - lastMode0Time) >= 50)) {
        colorWipe(strip.Color(255, 0, 0), strip.Color(0, 255, 0), strip.Color(0, 0, 255));
        lastMode0Time = millis();
    } else if (mode == 1 && ((millis() - lastMode1Time) >= 100)) {
        theaterChase(strip.Color(127, 127, 127), strip.Color(127, 0, 0), strip.Color(0, 0, 127)); // White
        lastMode1Time = millis();
    } else if (mode == 2 && ((millis() - lastMode2Time) >= 40)) {
        rainbow();
        lastMode2Time = millis();
    }

    baseIC.loop();
}
