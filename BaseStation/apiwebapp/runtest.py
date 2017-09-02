#!/usr/bin/env python3
import requests

headers = {'content-type': 'application/json'}

input("Press <enter> to create a peripheral")

create_url = 'http://127.0.0.1:8000/api/v1/peripherals/'
create_data = {'name': 'LED Light', 'address': '0013a200415537c5', 'queue': 'zigbee'}

r = requests.post(create_url, json=create_data, headers=headers)

print("URL: {}".format(create_url))
print("Data: {}".format(create_data))
print(repr(r))

input("Press <enter> to assign services [1,2,3] to the peripheral with address 0013a200415537c5")

assign_url = 'http://127.0.0.1:8000/api/v1/peripherals/0013a200415537c5/services/'
assign_data = {'services': ['1', '2', '3']}

r = requests.post(assign_url, json=assign_data, headers=headers)

print("URL: {}".format(assign_url))
print("Data: {}".format(assign_data))
print(repr(r))
