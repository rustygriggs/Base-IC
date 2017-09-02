# The various pieces necesary for all of Base-IC

- **BaseStation**
-- **apiwebapp** Django API manages the database of all peripherals as well as the IFTTT workflows that are attached to each peripheral and service. This is where the iOS and Android app will connect to manage and interace with the home automation system.
-- **apiworker** Worker that listens to Rabbit MQ and calls the applicable API endpoints when packets are received.
-- **zigbeeworker** Worker that listens to a serial port (with Zigbee attached) and translates any packets from the zigbee network and places them on Rabbit MQ to be sent to the API Worker.
- **HomeAutomationModule** Eagle layout for a module with a built in Arduino.
- **HomeAutomationModuleV2** Eagle layout for a module that attaches to an Arudino Nano.
- **LEDPeripheral** PlatformIO (Arduino) code that houses the BaseIC SDK for Arduino development.
- **OldBaseStation** Some experiments that didn't pan out.
