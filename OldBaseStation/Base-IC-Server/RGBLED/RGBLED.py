import RPi.GPIO as GPIO
import time


class RGBLED:
    def __init__(self):
        GPIO.setwarnings(False)  # Disable warnings of channels that are already in use
        GPIO.setmode(GPIO.BCM)  # set board mode to Broadcom

        GPIO.setup(22, GPIO.OUT)  # RED OUTPUT
        GPIO.setup(23, GPIO.OUT)  # GREEN OUTPUT
        GPIO.setup(24, GPIO.OUT)  # BLUE OUTPUT

    def flash_white(self, timeout=1):
        GPIO.output(22, 1)  # RED ON
        GPIO.output(23, 1)  # GREEN ON
        GPIO.output(24, 1)  # BLUE ON

        time.sleep(timeout)

        GPIO.output(22, 0)  # RED OFF
        GPIO.output(23, 0)  # GREEN OFF
        GPIO.output(24, 0)  # BLUE OFF
