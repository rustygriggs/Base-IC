#include "BaseIC.h"

/**
 * Constructor that sets up the software serial device.
 */
BaseIC::BaseIC(SoftwareSerial sSerial, bool verbose)
    : sSerial(sSerial), verbose(verbose) {}

/**
 * Initialization function.
 */
void BaseIC::begin() {
    sSerial.begin(9600);
    serialOutput[0] = '\0';

    /**
     * First set the XBee to API mode so that
     * the XBee library will work correctly.
     */
    setXBeeToAPIMode();

    xbee.setSerial(sSerial);
}

/*******************************************************************************
 ********************************** PUBLIC *************************************
 ******************************************************************************/

void BaseIC::attachListener(void (*listener)(ZBRxResponse &, uintptr_t)) {
    // Make sure that any errors are logged to Serial. The address of
    // Serial is first cast to Print*, since that's what the callback
    // expects, and then to uintptr_t to fit it inside the data parameter.
    // xbee.onPacketError(printErrorCb, (uintptr_t)(Print *) &Serial);
    // xbee.onTxStatusResponse(printErrorCb, (uintptr_t)(Print *) &Serial);
    // xbee.onZBTxStatusResponse(printErrorCb, (uintptr_t)(Print *) &Serial);

    // These are called when an actual packet received
    xbee.onZBRxResponse(listener);
}

void BaseIC::setNetworkID() {
    // Set the module network ID
    sendATCommand(idCmd, networkValue);
    sendATCommand(idCmd);

    // Write module settings
    sendATCommand(wrCmd);
}

void BaseIC::registerModule(uint8_t *name, uint8_t nameLength,
                            uint8_t *inputServices, uint8_t inputServicesCount,
                            uint8_t *outputServices, uint8_t outputServicesCount) {
    /**
     * Set the broadcast address of the coordinator.
     */
    addr64.setMsb(0x0000);
    addr64.setLsb(0xFFFF);
    zbTx.setAddress64(addr64);

    if (verbose) {
        Serial.print("Name Length: ");
        Serial.println(nameLength);
        Serial.print("Input Services Count: ");
        Serial.println(inputServicesCount);
        Serial.print("Output Services Count:");
        Serial.println(outputServicesCount);
    }

    // Register command:
    // 0\tname\tserviceType\tserviceType\n
    // 0\tname\tinputServicesCount\tserviceType\tserviceType\toutputServicesCount\tserviceType\n
    int payloadSize = 2 + nameLength + 2 + (inputServicesCount * 2) + 2 + (outputServicesCount * 2) + 1;
    int payloadIndex = 0;

    if (verbose) {
        Serial.print("Payload Size: ");
        Serial.println(payloadSize);
    }

    uint8_t *payload = (uint8_t *) malloc(payloadSize);
    payload[payloadIndex++] = '0';
    payload[payloadIndex++] = '\t';

    for (uint8_t i = 0; i < nameLength; i++) {
        if (verbose) {
            Serial.println((char) name[i]);
        }
        payload[payloadIndex++] = name[i];
    }

    payload[payloadIndex++] = '\t';

    payload[payloadIndex++] = (char) inputServicesCount + '0';
    payload[payloadIndex++] = '\t';
    for (uint8_t i = 0; i < inputServicesCount; i++) {
        payload[payloadIndex++] = inputServices[i];

        if (i < inputServicesCount - 1) {
            payload[payloadIndex++] = '\t';
        }
    }

    payload[payloadIndex++] = '\t';

    payload[payloadIndex++] = (char) outputServicesCount + '0';
    payload[payloadIndex++] = '\t';
    for (uint8_t i = 0; i < outputServicesCount; i++) {
        payload[payloadIndex++] = outputServices[i];

        if (i < outputServicesCount - 1) {
            payload[payloadIndex++] = '\t';
        }
    }

    payload[payloadIndex++] = '\n';

    if (verbose) {
        Serial.print("Payload Index: ");
        Serial.println(payloadIndex);

        for (uint8_t i = 0; i < payloadSize; i++) {
            Serial.print((char) payload[i]);
        }

        Serial.println();
    }

    zbTx.setPayload(payload);
    zbTx.setPayloadLength(payloadSize);

    xbee.send(zbTx);

    if (xbee.readPacket(500)) {
        // got a response!
        if (verbose) {
            Serial.println("Got a response!");
        }

        // should be a xbee tx status
        if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
            xbee.getResponse().getZBTxStatusResponse(txStatus);

            if (verbose) {
                // get the delivery status, the fifth byte
                if (txStatus.getDeliveryStatus() == SUCCESS) {
                    Serial.println("Success. Time to celebrate");
                } else {
                    Serial.println(
                        "Remote XBee did not receive our packet. Is it on?");
                }
            }
        }
    } else if (xbee.getResponse().isError()) {
        if (verbose) {
            Serial.print("Error reading packet.  Error code: ");
            Serial.println(xbee.getResponse().getErrorCode());
        }
    } else {
        if (verbose) {
            Serial.println("Local XBee did not provide a timely response");
            // local XBee did not provide a timely TX Status Response -- should
            // not happen
        }
    }

    free(payload);
}

void BaseIC::sendInt8(int serviceNumber, uint8_t value) {
    /**
     * Set the broadcast address of the coordinator.
     */
    addr64.setMsb(0x0000);
    addr64.setLsb(0xFFFF);
    zbTx.setAddress64(addr64);

    char hexVal[2];
    sprintf(hexVal, "%02X", value);

    // Payload will be of the format
    // serviceID\tserviceNumber\tvalue\n
    uint8_t payload[7];

    payload[0] = '1'; // Hex command
    payload[1] = '\t';
    payload[2] = serviceNumber + '0';
    payload[3] = '\t';
    payload[4] = hexVal[0];
    payload[5] = hexVal[1];
    payload[6] = '\n';

    if (verbose) {
        Serial.println("Begin Payload ---");
        for (uint8_t i = 0; i < 7; i++) {
            Serial.print((char) payload[i]);
        }
        Serial.println("\n--- End Payload");

        Serial.println();
    }

    zbTx.setPayload(payload);
    zbTx.setPayloadLength(7);

    xbee.sendAndWait(zbTx, XBEE_TIMEOUT);

    if (xbee.readPacket(500)) {
        // got a response!
        if (verbose) {
            Serial.println("Got a response!");
        }

        // should be a xbee tx status
        if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
            xbee.getResponse().getZBTxStatusResponse(txStatus);

            if (verbose) {
                // get the delivery status, the fifth byte
                if (txStatus.getDeliveryStatus() == SUCCESS) {
                    Serial.println("Success. Time to celebrate");
                } else {
                    Serial.println(
                        "Remote XBee did not receive our packet. Is it on?");
                }
            }
        }
    } else if (xbee.getResponse().isError()) {
        if (verbose) {
            Serial.print("Error reading packet.  Error code: ");
            Serial.println(xbee.getResponse().getErrorCode());
        }
    } else {
        if (verbose) {
            Serial.println("Local XBee did not provide a timely response");
            // local XBee did not provide a timely TX Status Response -- should
            // not happen
        }
    }
}

void BaseIC::loop() { xbee.loop(); }

/*******************************************************************************
 ********************************** PRIVATE ************************************
 ******************************************************************************/

/**
 * Reset the XBee module and set it to API mode.
 */
void BaseIC::setXBeeToAPIMode() {
    // Enter AT command mode
    if (verbose) {
        Serial.println("Enter AT Command Mode");
    }

    sendSerialCommand("+++", "OK");

    if (verbose) {
        Serial.println("---");
    }

    // Reset the module
    if (verbose) {
        Serial.println("Reset the module");
    }

    sendSerialCommand("ATRE\r", "OK");

    if (verbose) {
        Serial.println("---");
    }

    // Set module to API mode 2
    if (verbose) {
        Serial.println("Set module to API mode 2");
    }

    sendSerialCommand("ATAP 2\r", "OK");

    if (verbose) {
        Serial.println("---");
    }

    // Write settings to XBee
    if (verbose) {
        Serial.println("Write settings to XBee");
    }

    sendSerialCommand("ATWR\r", "OK");

    if (verbose) {
        Serial.println("---");
    }

    // Drop out of command mode
    if (verbose) {
        Serial.println("Drop out of command mode");
    }

    sendSerialCommand("ATCN\r", "OK");

    if (verbose) {
        Serial.println("---");
    }
}

/**
 * Send a command to the XBee via raw serial commands and wait for
 * the expected output from the module.
 */
void BaseIC::sendSerialCommand(String command, String expectedOutput) {
    serialOutput[0] = '\0';
    serialOutputIndex = 0;

    if (verbose) {
        Serial.println(command);
    }

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

    if (verbose) {
        Serial.println(serialOutput);
    }
}

void BaseIC::sendATCommand(uint8_t *cmd, uint8_t *value) {
    if (verbose) {
        Serial.println("Sending command to XBee");
    }
    atRequest.setCommand(idCmd);

    if (value != NULL) {
        if (verbose) {
            Serial.println("Sending command with value");
        }
        atRequest.setCommandValue(value);
        atRequest.setCommandValueLength(2);
    } else {
        atRequest.setCommandValue(NULL);
        atRequest.setCommandValueLength(0);
    }

    xbee.send(atRequest);

    if (xbee.readPacket(5000)) {
        if (verbose) {
            Serial.println("Got a response");
        }

        // should be an AT command response
        if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
            xbee.getResponse().getAtCommandResponse(atResponse);

            if (atResponse.isOk()) {
                if (verbose) {
                    Serial.print("Command [");
                    Serial.print(atResponse.getCommand()[0], HEX);
                    Serial.print(atResponse.getCommand()[1], HEX);
                    Serial.println("] was successful!");
                }

                if (verbose && (atResponse.getValueLength() > 0)) {
                    Serial.print("Command value length is ");
                    Serial.println(atResponse.getValueLength(), DEC);

                    Serial.print("Command value: ");

                    for (int i = 0; i < atResponse.getValueLength(); i++) {
                        Serial.print(atResponse.getValue()[i], HEX);
                        Serial.print(" ");
                    }

                    Serial.println("");
                }
            } else {
                if (verbose) {
                    Serial.print("Command return error code: ");
                    Serial.println(atResponse.getStatus(), HEX);
                }
            }
        } else {
            if (verbose) {
                Serial.print("Expected AT response but got ");
                Serial.print(xbee.getResponse().getApiId(), HEX);
            }
        }
    } else {
        if (verbose) {
            Serial.println("Timeout");
            // at command failed
            if (xbee.getResponse().isError()) {
                Serial.print("Error reading packet.  Error code: ");
                Serial.println(xbee.getResponse().getErrorCode());
            } else {
                Serial.print("No response from radio");
            }
        }
    }
}
