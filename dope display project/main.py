import wifimgr  # wifi library
# essential raspberry pi pico libraries
from time import sleep
import machine
import gc
# screen library
import max7219
from machine import SPI, Pin
import random
import urequests as requests  # correct import for microcontrollers
spi = SPI(1)
display = max7219.Matrix8x8(spi, Pin('X5'), 4)
# Purple led
Purple = machine.Pin(3, machine.Pin.OUT)
# initializing wifi setup
wlan = wifimgr.get_connection()  # initializing wlan
api_key = 'REPLACE_WITH_YOUR_API_KEY'
location = 'guatemala'
url = f'https://api.weatherapi.com/v1/current.json?q={location}+&key={api_key}'
delays = [1, 4, 10, 8, 9, 6, 11]
# if there's no wifi
if wlan is None:
    print("Could not initialize the network connection.")
    while True:
        pass
# The actual code now
print("wifi ok")
while True:

    delay = delays[random.randint(0, 6)]
    try:
        response = requests.get(url)
        # Print the response code
        print('Response code: ', response.status_code)
        weather = response.json()
        # Close the request
        response.close()
        print('Weather JSON: ', weather)
    except Exception as e:
        print('Failed to fetch weather data:', e)
    sleep(delay * 60)
