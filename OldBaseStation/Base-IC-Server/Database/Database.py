import sqlite3


class Database:
    def __init__(self):
        print("Begin DB")
        self.conn = sqlite3.connect('db/base-ic.db')
        self._create_populate_tables()
        self.services = {
            1: 'Hex',
            2: 'Range',
            3: 'Toggle',
            4: 'Read'
        }

    def find_create_peripheral(self, name):
        pass

    def add_services_to_peripheral(self, peripheral_id, services=[]):
        pass

    def _create_populate_tables(self):
        """Create the initial empty tables for the base station"""
        cursor = self.conn.cursor()

        # Create the peripherals table if it doesn't exist.
        cursor.execute('''CREATE TABLE IF NOT EXISTS
                           peripherals(id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR(255));''')

        # Create the peripherals_services table if it doesn't exist.
        cursor.execute('''CREATE TABLE IF NOT EXISTS
                          peripherals_services(peripheral_id INTEGER, service_id INTEGER);''')

    def __del__(self):
        self.conn.close()
        print("Close DB")
