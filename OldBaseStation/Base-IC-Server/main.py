import serial
from XBeeSerial.XBeeSerial import XBeeSerial
from XBeeSerial.XBeeSerial import RegistrationRequest
from Database.Database import Database


# from RGBLED.RGBLED import RGBLED


def main():
    """The main routine."""

    # Create the LED control and flash the white light for 1 second.
    # rgbled = RGBLED()
    # rgbled.flash_white()

    # Init the xbee serial communication and set the module to API mode and as a coordinator.
    try:
        xbee_serial = XBeeSerial()
    except serial.serialutil.SerialException:
        print("Serial not found for Arduino, trying Mac port")
        xbee_serial = XBeeSerial(port='/dev/cu.usbserial-A603U21W')

    # Open a connection to the sqlite DB
    database = Database()

    while True:
        try:
            request = xbee_serial.wait_poll_frame()
            print(request)
            if type(request) is RegistrationRequest:
                peripheral_id = database.find_create_peripheral(request.get_name())
                database.add_services_to_peripheral(peripheral_id, request.get_services())
        except KeyboardInterrupt:
            break


if __name__ == "__main__":
    main()
