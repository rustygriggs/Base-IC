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
uint8_t services[4] = {'1', '2', '3', '4'};
uint8_t name[9] = {'L', 'E', 'D', ' ', 'L', 'I', 'G', 'H', 'T'};

void setup()
{
  // pinMode(13, OUTPUT);
  Serial.begin(9600);

  baseIC.begin();

  baseIC.setNetworkID();

  baseIC.registerModule(name, sizeof(name), services, sizeof(services));

  /*
//
// xbee.tx(
//   dest_addr_long='\x00\x13\xa2\x00\x41\x55\x37\xc5',
//   dest_addr='\xff\xfe',
//   data='Hello Back'
// )
// print xbee.wait_read_frame()
*/
}

/*
+++
ATRE
ATAP 2
ATWR
ATCN
 */

void loop()
{
  /*
  if (sSerial.available()) {
    Serial.write(sSerial.read());
  }
  if (Serial.available()) {
    sSerial.write(Serial.read());
  }
  */
}
