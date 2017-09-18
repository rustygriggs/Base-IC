#include <Arduino.h>
#include <XBee.h>
#include <Printers.h>
#include <SoftwareSerial.h>

class BaseIC {
  SoftwareSerial sSerial;

  char serialOutput[10];
  char curChar;
  bool verbose;

  int serialOutputIndex = 0;

  XBeeWithCallbacks xbee;

  uint8_t idCmd[2] = {'I', 'D'};
  uint8_t networkValue[2] = {0xBE, 0xEF};

  uint8_t ceCmd[2] = {'C', 'E'};
  uint8_t threeValue[1] = {3};

  uint8_t wrCmd[2] = {'W', 'R'};

  uint8_t helloBackValue[10] = {'H', 'E', 'L', 'L', 'O', ' ', 'B', 'A', 'C', 'K'};

  XBeeAddress64 addr64 = XBeeAddress64();
  ZBTxRequest zbTx = ZBTxRequest();
  ZBTxStatusResponse txStatus = ZBTxStatusResponse();
  AtCommandRequest atRequest = AtCommandRequest();
  AtCommandResponse atResponse = AtCommandResponse();

public:
  BaseIC(SoftwareSerial sSerial, bool verbose = false);
  void begin();
  void setNetworkID();
  void registerModule(
    uint8_t * name, uint8_t nameLength,
    uint8_t * inputServices, uint8_t inputServicesCount,
    uint8_t * outputServices, uint8_t outputServicesCount
  );
  void loop();
  void attachListener(void (*listener)(ZBRxResponse &, uintptr_t));
  static void zbReceive(ZBRxResponse& rx, uintptr_t);

private:
  void setXBeeToAPIMode();
  void sendSerialCommand(String command, String expectedOutput);
  void sendATCommand(uint8_t *cmd, uint8_t *value = NULL);
};
