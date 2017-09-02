from celery import Celery
from zigbee import Zigbee
from baseicapi import apiSendCommand, apiReceiveCommand
import time

app = Celery('baseiczigbee', broker='pyamqp://guest@localhost//')

zigbee = Zigbee()

@app.task
def zigbeeSendCommand():
    zigbee.sendCommand()

@app.task
def zigbeeReceiveCommand():
    zigbee.receiveCommand()

while True:
    zigbeeSendCommand()
    time.sleep(10)
