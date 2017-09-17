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

void responseListener(
  int outputServiceNumber, int outputServiceId,
  uint8_t valueLength, uint8_t * value
) {
  Serial.print("Output Service Number: ");
  Serial.println(outputServiceNumber);
  Serial.println("Output Service ID: ");
  Serial.println(outputServiceId);
  Serial.println("Value");
  for (int i = 0; i < valueLength; i++) {
    Serial.print((char) value[i]);
  }
  Serial.println();
}

void setup()
{
  // pinMode(13, OUTPUT);
  Serial.begin(9600);

  baseIC.begin();

  baseIC.setNetworkID();

  baseIC.registerModule(
    name, sizeof(name),
    inputServices, sizeof(inputServices),
    outputServices, sizeof(outputServices)
  );

  // Now we should be able to attach a callback handler that gets called
  // whenever the module receives anything from the BaseStation
  baseIC.startListening(&responseListener);
}

void loop() {/* Should never get here */}
