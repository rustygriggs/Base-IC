from celery import Celery
from api import Api
from baseiczigbee import zigbeeSendCommand, zigbeeReceiveCommand
import time

app = Celery('baseicapi', broker='pyamqp://guest@localhost//')

api = Api()

@app.task
def apiSendCommand():
    api.sendZigbeeCommand()

@app.task
def apiReceiveCommand():
    api.receiveZigbeeCommand()

while True:
    zigbeeSendCommand()
    time.sleep(10)
