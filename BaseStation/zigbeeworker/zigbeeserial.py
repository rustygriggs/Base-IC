from xbee import ZigBee
import serial
from queue import Queue
import binascii


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


class ZigbeeSerial:
    def __init__(self, rx_queue: Queue, tx_queue: Queue, port='/dev/ttyS0', baudrate=9600):
        self.ser = serial.Serial(port=port, baudrate=baudrate)

        # And begin listening for packets
        self.xbee = ZigBee(self.ser, escaped=True, callback=self.zigbee_callback)

        # Store the reference to the queues
        self.rx_queue = rx_queue
        self.tx_queue = tx_queue

        # After the serial port has been opened then configure the ZigBee module
        self.set_api_mode()
        self.set_as_coordinator()

    def __del__(self):
        print("Closing serial")

        if hasattr(self, 'ser'):
            self.ser.close()

    def set_api_mode(self):
        """
        Set the attached zigbee module to API mode so that it will work with the ZigBee module
        """

        # Enter AT command mode
        print("+++")
        self.ser.write(b"+++")
        # self.print_serial(3)
        # Reset the module
        print("Reset Module")
        self.ser.write(b"ATRE\r\n")
        # self.print_serial(3)
        # Set the module into API mode with escaping
        print("Set API mode to 2")
        self.ser.write(b"ATAP 2\r\n")
        # self.print_serial(3)
        # Write the settings to the module
        print("Write settings")
        self.ser.write(b"ATWR\r\n")
        # self.print_serial(3)
        print("Drop out of command mode")
        self.ser.write(b"ATCN\r\n")
        # self.print_serial(3)

    def set_as_coordinator(self):
        """
        Since this code is to be run on the base station only we can assume that
        this zigbee module will be the coordinator and we can set it as such
        """

        # Set the module network ID
        print("Setting network ID")
        self.xbee.at(command=b'ID', parameter=b'\xbe\xef')
        # print(self.xbee.wait_read_frame())
        print("Network ID set to")
        self.xbee.at(command=b'ID')
        # print(self.xbee.wait_read_frame())

        # Set this module as the Non-Routing Coordinator
        print("Setting module type")
        self.xbee.at(command=b'CE', parameter=b'3')
        # print(self.xbee.wait_read_frame())
        print("Module set as ")
        self.xbee.at(command=b'CE')
        # print(self.xbee.wait_read_frame())

    def zigbee_callback(self, data):
        """
        We will need two things from the data every time. First is the long address and the second is the rf_data.
        Anything that was received over the serial connection will be slightly parsed and then sent to the queue.
        """

        print('zigbee_callback')

        # The data will only contain source_addr_long when it comes from an external zigbee, and not a setup command.
        if 'source_addr_long' in data:
            # Decode the source address
            source_address = binascii.hexlify(data['source_addr_long']).decode()
            print(source_address)

            # Take the received rf data, decode it as UTF-8, remove the \n from the end of the string and split on tabs.
            print(data['rf_data'])
            print(data['rf_data'].decode())
            parsed_data = data['rf_data'].decode()[:-1].split("\t")
            print(parsed_data)

            self.rx_queue.put({
                'address': source_address,
                'data': parsed_data,
            })
        else:
            # If this is a setup command then just print the data.
            print(data)
