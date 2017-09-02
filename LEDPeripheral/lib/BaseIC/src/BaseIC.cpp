#include "BaseIC.h"

/**
 * Constructor that sets up the software serial device.
 */
BaseIC::BaseIC(SoftwareSerial sSerial, bool verbose) :
  sSerial(sSerial), verbose(verbose) {}

/**
 * Initialization function.
 */
void BaseIC::begin()
{
  sSerial.begin(9600);
  serialOutput[0] = '\0';

  /**
   * First set the XBee to API mode so that
   * the XBee library will work correctly.
   */
  setXBeeToAPIMode();
}

/*******************************************************************************
 ********************************** PUBLIC *************************************
 ******************************************************************************/

void BaseIC::setNetworkID()
{
  xbee.setSerial(sSerial);

  // Set the module network ID
  sendATCommand(idCmd, networkValue);
  sendATCommand(idCmd);

  // Write module settings
  sendATCommand(wrCmd);
}

void BaseIC::registerModule(uint8_t * name, uint8_t nameLength, uint8_t * services, uint8_t servicesCount)
{
  xbee.setSerial(sSerial);

  /**
   * Set the broadcast address of the coordinator.
   */
  // addr64.setMsb(0x0013a200);
  // addr64.setLsb(0x415537C4);
  addr64.setMsb(0x0000);
  addr64.setLsb(0xFFFF);
  zbTx.setAddress64(addr64);

  // Register command:
  // 0\tname\tserviceType\tserviceType\n
  int payloadSize = 2 + nameLength + 1 + (servicesCount * 2);
  int payloadIndex = 0;

  Serial.print("Payload Size: ");
  Serial.println(payloadSize);

  uint8_t * payload = (uint8_t *) malloc(payloadSize);
  payload[payloadIndex++] = '0';
  payload[payloadIndex++] = '\t';

  for (uint8_t i = 0; i < nameLength; i++) {
    Serial.println((char) name[i]);
    payload[payloadIndex++] = name[i];
  }

  payload[payloadIndex++] = '\t';

  for (uint8_t i = 0; i < servicesCount; i++) {
    payload[payloadIndex++] = services[i];

    if (i < servicesCount - 1) {
      payload[payloadIndex++] = '\t';
    }
  }

  payload[payloadIndex++] = '\n';

  Serial.print("Payload Index: ");
  Serial.println(payloadIndex);

  for (uint8_t i = 0; i < payloadSize; i++) {
    Serial.print((char) payload[i]);
  }

  Serial.println();

  zbTx.setPayload(payload);
  zbTx.setPayloadLength(payloadSize);

  xbee.send(zbTx);

  if (xbee.readPacket(500)) {
    Serial.println("Got a response!");
    // got a response!

    // should be a znet tx status
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);

      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        Serial.println("Success. Time to celebrate");
        // flashLed(statusLed, 5, 50);
      } else {
        Serial.println("Remove XBee did not receive our packet. Is it on?");
        // flashLed(errorLed, 3, 500);
      }
    }
  } else if (xbee.getResponse().isError()) {
    Serial.print("Error reading packet.  Error code: ");
    Serial.println(xbee.getResponse().getErrorCode());
  } else {
    Serial.println("Local XBee did not provide a timely response");
    // local XBee did not provide a timely TX Status Response -- should not happen
    // flashLed(errorLed, 2, 50);
  }
}

/*******************************************************************************
 ********************************** PRIVATE ************************************
 ******************************************************************************/

/**
 * Reset the XBee module and set it to API mode.
 */
void BaseIC::setXBeeToAPIMode()
{
  // Enter AT command mode
  if (verbose) Serial.println("Enter AT Command Mode");
  sendSerialCommand("+++", "OK");
  if (verbose) Serial.println("---");
  // Reset the module
  if (verbose) Serial.println("Reset the module");
  sendSerialCommand("ATRE\r", "OK");
  if (verbose) Serial.println("---");
  // Set module to API mode 2
  if (verbose) Serial.println("Set module to API mode 2");
  sendSerialCommand("ATAP 2\r", "OK");
  if (verbose) Serial.println("---");
  // Write settings to XBee
  if (verbose) Serial.println("Write settings to XBee");
  sendSerialCommand("ATWR\r", "OK");
  if (verbose) Serial.println("---");
  // Drop out of command mode
  if (verbose) Serial.println("Drop out of command mode");
  sendSerialCommand("ATCN\r", "OK");
  if (verbose) Serial.println("---");
}

/**
 * Send a command to the XBee via raw serial commands and wait for
 * the expected output from the module.
 */
void BaseIC::sendSerialCommand(String command, String expectedOutput)
{
  serialOutput[0] = '\0';
  serialOutputIndex = 0;

  Serial.println(command);
  sSerial.print(command);
  while (strcmp(serialOutput, "OK")) {
    if (sSerial.available()) {
      curChar = sSerial.read();
      if (curChar != '\r') {
        serialOutput[serialOutputIndex++] = curChar;
        serialOutput[serialOutputIndex] = '\0';
      }
    }
  }

  Serial.println(serialOutput);
}

void BaseIC::sendATCommand(uint8_t *cmd, uint8_t *value) {
  Serial.println("Sending command to XBee");
  atRequest.setCommand(idCmd);

  if (value != NULL) {
    Serial.println("Sending command with value");
    atRequest.setCommandValue(value);
    atRequest.setCommandValueLength(2);
  } else {
    atRequest.setCommandValue(NULL);
    atRequest.setCommandValueLength(0);
  }

  xbee.send(atRequest);

  if (xbee.readPacket(5000)) {
    Serial.println("Got a response");

    // should be an AT command response
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      xbee.getResponse().getAtCommandResponse(atResponse);

      if (atResponse.isOk()) {
        Serial.print("Command [");
        Serial.print(atResponse.getCommand()[0], HEX);
        Serial.print(atResponse.getCommand()[1], HEX);
        Serial.println("] was successful!");

        if (atResponse.getValueLength() > 0) {
          Serial.print("Command value length is ");
          Serial.println(atResponse.getValueLength(), DEC);

          Serial.print("Command value: ");

          for (int i = 0; i < atResponse.getValueLength(); i++) {
            Serial.print(atResponse.getValue()[i], HEX);
            Serial.print(" ");
          }

          Serial.println("");
        }
      }
      else {
        Serial.print("Command return error code: ");
        Serial.println(atResponse.getStatus(), HEX);
      }
    } else {
      Serial.print("Expected AT response but got ");
      Serial.print(xbee.getResponse().getApiId(), HEX);
    }
  } else {
    Serial.println("Timeout");
    // at command failed
    if (xbee.getResponse().isError()) {
      Serial.print("Error reading packet.  Error code: ");
      Serial.println(xbee.getResponse().getErrorCode());
    }
    else {
      Serial.print("No response from radio");
    }
  }

  // xbee = ZigBee(ser, escaped=True)
  //
  // # Set the module network ID
  // print "Setting network ID"
  // xbee.at(command='ID', parameter='\xbe\xef')
  // print xbee.wait_read_frame()
  // print "Network ID set to"
  // xbee.at(command='id')
  // print xbee.wait_read_frame()
}
