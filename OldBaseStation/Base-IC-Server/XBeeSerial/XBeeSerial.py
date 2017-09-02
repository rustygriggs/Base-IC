from xbee import ZigBee
import serial, time


class RegistrationRequest:
    def __init__(self, parsed_data):
        self.name = parsed_data[1]
        self.services = []

        for i in range(2, len(parsed_data)):
            print(i, parsed_data[i])
            if parsed_data[i] == '\x01':
                self.services.append(1)
            elif parsed_data[i] == '\x02':
                self.services.append(2)
            elif parsed_data[i] == '\x03':
                self.services.append(3)
            elif parsed_data[i] == '\x04':
                self.services.append(4)

    def get_name(self):
        return self.name

    def get_services(self):
        return self.services

    def print_services(self):
        for i in range(len(self.services)):
            if self.services[i] == 1:
                print("Service", i, "is a hex value")
            elif self.services[i] == 2:
                print("Service", i, "is a range value")
            elif self.services[i] == 3:
                print("Service", i, "is a toggle value")
            elif self.services[i] == 4:
                print("Service", i, "is a read command")


class XBeeSerial:
    def __init__(self, port='/dev/ttyS0', baudrate=9600):
        self.ser = serial.Serial(port=port, baudrate=baudrate)
        self.xbee = ZigBee(self.ser, escaped=True)

        self.set_api_mode()
        self.set_as_coordinator()

    def __del__(self):
        print("Closing serial")

        if hasattr(self, 'ser'):
            self.ser.close()

    def set_api_mode(self):
        # Enter AT command mode
        self.ser.write(b"+++")
        self.print_serial(3)
        # Reset the module
        print("Reset Module")
        self.ser.write(b"ATRE\r\n")
        self.print_serial(3)
        # Set the module into API mode with escaping
        print("Set API mode to 2")
        self.ser.write(b"ATAP 2\r\n")
        self.print_serial(3)
        # Write the settings to the module
        print("Write settings")
        self.ser.write(b"ATWR\r\n")
        self.print_serial(3)
        print("Drop out of command mode")
        self.ser.write(b"ATCN\r\n")
        self.print_serial(3)

    def set_as_coordinator(self):
        # Set the module network ID
        print("Setting network ID")
        self.xbee.at(command=b'ID', parameter=b'\xbe\xef')
        print(self.xbee.wait_read_frame())
        print("Network ID set to")
        self.xbee.at(command=b'ID')
        print(self.xbee.wait_read_frame())

        # Set this module as the Non-Routing Coordinator
        print("Setting module type")
        self.xbee.at(command=b'CE', parameter=b'3')
        print(self.xbee.wait_read_frame())
        print("Module set as ")
        self.xbee.at(command=b'CE')
        print(self.xbee.wait_read_frame())

    def print_serial(self, bytes):
        self.ser.flush()
        time.sleep(.1)
        buffer_bytes = self.ser.in_waiting
        if buffer_bytes == 0:
            print(self.ser.read(bytes))
        else:
            print(self.ser.read(buffer_bytes))

    def wait_poll_frame(self):
        data = self.xbee.wait_read_frame()
        decoded_data = data['rf_data'].decode("utf-8")
        parsed_data = decoded_data.replace('\n', '').split('\t')
        if parsed_data[0] == '0':
            return RegistrationRequest(parsed_data)
