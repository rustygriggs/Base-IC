#include <Arduino.h>
#include <BaseIC.h>
#include <SoftwareSerial.h>

SoftwareSerial sSerial(8, 9); // RX, TX

BaseIC baseIC = BaseIC(sSerial, true);

/**
 * Register a hex, range, toggle, and read service.
 * Everthing needs to be characters. The zigbee module encoded and decodes
 * characters better than it does integers.
 */
uint8_t inputServices[1] = {'3'}; // Define one Toggle Input
uint8_t outputServices[1] = {'3'}; // Define one Toggle Output
uint8_t name[9] = {'L', 'E', 'D', ' ', 'L', 'I', 'G', 'H', 'T'};

void responseListener(ZBRxResponse &rx, uintptr_t)
{
  // We are going to get three values when we receive a response.
  // First: Output Service ID
  // Second: Ouput Service Number
  // Third: Output Service Value
  // The format will be OutputServiceID\tOutputServiceName\tOutputServiceValue\n
  int outputServiceId = rx.getData(0) - '0';
  int outputServiceNumber = rx.getData(2) - '0';
  char * outputServiceValue = (char *) malloc(sizeof(char) * (rx.getDataLength() - 3));
  int i = 4;
  for (; i < rx.getDataLength(); i++) {
    outputServiceValue[i - 4] = rx.getData(i);
  }
  outputServiceValue[i] = '\0';

  // If the response is for the toggle service and is service number 1.
  // THEN TOGGLE!!!
  if (outputServiceId == 3 && outputServiceNumber == 1) {
    if (digitalRead(13) == HIGH) {
      digitalWrite(13, LOW);
    } else {
      digitalWrite(13, HIGH);
    }
  }

  Serial.print("Output Service ID: ");
  Serial.println(outputServiceId);
  Serial.print("Output Service Number: ");
  Serial.println(outputServiceNumber);
  Serial.print("Output Service Value: ");
  Serial.println(outputServiceValue);

  free(outputServiceValue);
}

void setup()
{
  pinMode(13, OUTPUT);
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
}
