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

# Initialize the SPI bus and display
spi = SPI(1)
display = max7219.Matrix8x8(spi, Pin('X5'), 4)

# Purple LED
Purple = machine.Pin(3, machine.Pin.OUT)

# Initializing WiFi setup
wlan = wifimgr.get_connection()  # initializing WLAN
api_key = ''
mode = 'quotes'
url = f'https://zenquotes.io/api/{mode}?option1=value&option2=value'
delays = [1, 4, 10, 8, 9, 6, 11]

# If there's no WiFi
if wlan is None:
    print("Could not initialize the network connection.")
    while True:
        pass

# The actual code now
print("WiFi OK")
while True:
    delay = delays[random.randint(0, 6)]
    try:
        response = requests.get(url)
        # Print the response code
        print('Response code: ', response.status_code)
        quotes = response.json()
        # Close the request
        response.close()
        print('quotes JSON: ', quotes)

        # Extract the temperature from the JSON response
        quote = quotes['current']['temp_c']
       quotes_text = f'Temperature: {temperature}°C'

        # Scroll the temperature on the display
        for i in range(len(quotes_text) * 8):
            display.fill(0)
            display.text(quotes_text, -i, 0, 1)
            display.show()
            sleep(0.1)

    except Exception as e:
        print('Failed to fetch quote data:', e)
        
    sleep(delay * 60)
