#include <Arduino.h>
#include <BaseIC.h>
#include <SoftwareSerial.h>

SoftwareSerial sSerial(8, 9); // RX, TX

#define VERBOSE false
// which analog pin to connect
#define THERMISTORPIN A0
// resistance at 25 degrees C
#define THERMISTORNOMINAL 100000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 9900
// button pins
#define UP 3
#define DOWN 2
#define MODE 4
#define FAN 5
#define HEATER 10
#define AC 11
#define AFAN 12
#define DELAY 50

#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Using version 1.2.1

// The LCD constructor - address shown is 0x27
// Also based on YWRobot LCM1602 IIC V1
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

int counter = 0; // counter for temp update. So it only updates once a second or so

int setTemp = 75;

int heaterState = 0;
int acState = 0;
int aFanState = 0;

bool fanState = true;
int modeState = 1;

// button states
int BS2;
int BS3;
int BS4;
int BS5;
//last button states
int LBS2 = LOW;
int LBS3 = LOW;
int LBS4 = LOW;
int LBS5 = LOW;

// last debounced times
unsigned long LDT2 = 0;
unsigned long LDT3 = 0;
unsigned long LDT4 = 0;
unsigned long LDT5 = 0;

BaseIC baseIC = BaseIC(sSerial, VERBOSE);

/**
 * Register a hex, range, toggle, and read service.
 * Everthing needs to be characters. The zigbee module encoded and decodes
 * characters better than it does integers.
 */
uint8_t inputServices[3] = {'3', '3', '3'}; // Define one Toggle Input
uint8_t outputServices[1] = {'1'}; // Define one Hex Output
uint8_t name[10] = {'T', 'h', 'e', 'r', 'm', 'o', 's', 't', 'a', 't'};

void changeMode()
{
    if(modeState == 0)
    {
      modeState = 1;
    }
    else if(modeState == 1)
    {
      heaterState = 1;
      acState = 0;

      modeState = 2;
    }
    else if(modeState == 2)
    {
      heaterState = 0;
      acState = 1;

      modeState = 0;
    }
}

void changeFan(){
    if(!fanState)
    {
      aFanState = 0;
    }
    else
    {
      aFanState = 1;
    }
    fanState = !fanState;
}

void debounceButton(int buttonRead, int* lastButtonState, unsigned long* lastDebounceTime, int* buttonState)
{
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonRead);
  int selector = buttonRead; // selects what we want to do when the button is pressed
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
        if(selector == DOWN)
        {
          setTemp -= 1;
        }
        else if(selector == UP)
        {
          setTemp += 1;
        }
        else if(selector == MODE)
        {
          changeMode();
        }
        else if(selector == FAN)
        {
          changeFan();
        }
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

    // If the response is for the toggle service and is service number 1.
    // THEN TOGGLE!!!
    if (serviceId == 1 && serviceNumber == 1) {
        Serial.print("Value: ");
        Serial.println(buffer);
        if(strcmp(buffer, "01") == 0){
            setTemp = 80;
            modeState = 1;
            fanState = 0;
        }
        else if(strcmp(buffer, "02") == 0){
            setTemp = 68;
            modeState = 2;
            fanState = 0;
        }
        else if(strcmp(buffer, "03") == 0){
            setTemp = 75;
            modeState = 0;
            fanState = 0;
        }
        changeMode();
        changeFan();
    }
}


void setup() {
    pinMode(DOWN, INPUT);
    pinMode(UP, INPUT);
    pinMode(MODE, INPUT);
    pinMode(FAN, INPUT);
    pinMode(HEATER, OUTPUT);
    pinMode(AC, OUTPUT);
    pinMode(AFAN, OUTPUT);
    lcd.begin(16,2); // sixteen characters across - 2 lines
    lcd.backlight();

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
    debounceButton(DOWN, &LBS2, &LDT2, &BS2);
    debounceButton(UP, &LBS3, &LDT3, &BS3);
    debounceButton(MODE, &LBS4, &LDT4, &BS4);
    debounceButton(FAN, &LBS5, &LDT5, &BS5);

    // Continuously let xbee read packets and call callbacks.
    baseIC.loop();

    float temp;

    temp = analogRead(THERMISTORPIN);

    // convert the value to resistance
    temp = (1023 / temp)  - 1;     // (1023/ADC - 1)
    temp = SERIESRESISTOR / temp;  // 10K / (1023/ADC - 1)

    float currentTemp;
    currentTemp = temp / THERMISTORNOMINAL;     // (R/Ro)
    currentTemp = log(currentTemp);                  // ln(R/Ro)
    currentTemp /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
    currentTemp += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
    currentTemp = 1.0 / currentTemp;                 // Invert
    currentTemp -= 273.15;                         // convert to C

    currentTemp = currentTemp*(1.8) + 32;

    // if its not off
    if(modeState != 1){
      if((currentTemp + 1) < setTemp)
      {
        if(modeState == 2)
        {
          digitalWrite(HEATER, HIGH);
          digitalWrite(AC, LOW);
          digitalWrite(AFAN, HIGH);
        }
        else if(modeState == 0)
        {
          digitalWrite(HEATER, LOW);
          digitalWrite(AC, LOW);
          digitalWrite(AFAN, aFanState);
        }
      }
      else if((currentTemp - 1) > setTemp)
      {
        if(modeState == 2)
        {
          digitalWrite(HEATER, LOW);
          digitalWrite(AC, LOW);
          digitalWrite(AFAN, aFanState);
        }
        else if(modeState == 0)
        {
          digitalWrite(HEATER, LOW);
          digitalWrite(AC, HIGH);
          digitalWrite(AFAN, HIGH);
        }
      }
      else
      {
        digitalWrite(HEATER, LOW);
        digitalWrite(AC, LOW);
        digitalWrite(AFAN, aFanState);
      }
    }
    else{
      digitalWrite(HEATER, LOW);
      digitalWrite(AC, LOW);
      digitalWrite(AFAN, LOW);
    }

    if(counter++ % 100 == 0)
    {
      lcd.setCursor(0,0);
      lcd.print(currentTemp);
      lcd.print(" F");
    }

    lcd.setCursor(10,0);
    lcd.print(setTemp);
    lcd.print(" F");

    lcd.setCursor(0,1);
    if(modeState == 1)
    {
        lcd.print("SYS:OFF ");
    }
    else if(modeState == 2)
    {
        lcd.print("SYS:HEAT");
    }
    else if(modeState == 0)
    {
        lcd.print("SYS:COOL");
    }




    lcd.setCursor(9,1);

    if(!fanState){
        lcd.print("FAN:ON  ");
    }
    else{
        lcd.print("FAN:AUTO  ");
    }


}
