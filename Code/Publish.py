# Import standard python modules.
import sys
import time
import serial

# Uses the MQTTClient
from Adafruit_IO import MQTTClient

ADAFRUIT_IO_USERNAME = "rudy_04"
ADAFRUIT_IO_KEY = "aio_Qkdj59MFG8owknAHr2Z0ZuCXC5nG"

# Set to the ID of the feed to subscribe to for updates.
feedState = 'state'
feedBase = 'base'
feedBrazo = 'brazo'
feedGarra = 'garra'
feedMuneca = 'muneca'

feedP1 = 'posicion1'
feedP2 = 'posicion2'
feedP3 = 'posicion3'
feedP4 = 'posicion4'

mensaje = ''

# Define callback functions which will be called when certain events happen.
def connected(client):
    # Subscribe to changes on a feed
    client.subscribe(feedState)
    client.subscribe(feedBase)
    client.subscribe(feedBrazo)
    client.subscribe(feedGarra)
    client.subscribe(feedMuneca)
    client.subscribe(feedP1)
    client.subscribe(feedP2)
    client.subscribe(feedP3)
    client.subscribe(feedP4)
    client.publish(feedState, 1)

def disconnected(client):
    """Disconnected function will be called when the client disconnects."""
    sys.exit(1)

def message(client, feed_id, payload):
    """Message function will be called when a subscribed feed has a new value.
    The feed_id parameter identifies the feed, and the payload parameter has
    the new value.
    """
    print('Feed {0} received new value: {1}'.format(feed_id, payload))
    
    if feed_id in [feedP1, feedP2, feedP3, feedP4]:
        handle_button_press(feed_id, payload)
    elif feed_id in [feedState, feedBase, feedBrazo, feedGarra, feedMuneca]:
        handle_servo_move(feed_id, payload)

def handle_button_press(feed_id, payload):
    if payload == '1':
        arduino.write(bytes('A\n', 'utf-8'))
    elif payload == '2':
        arduino.write(bytes('B\n', 'utf-8'))
    elif payload == '3':
        arduino.write(bytes('Q\n', 'utf-8'))
        client.publish(feedState, 1)
    elif payload == '4':
        arduino.write(bytes('W\n', 'utf-8'))
        client.publish(feedState, 2)
    elif payload == '5':
        arduino.write(bytes('E\n', 'utf-8'))
        client.publish(feedState, 3)

def handle_servo_move(feed_id, payload):
    feed_to_char = {
        feedBase: 'c',
        feedBrazo: 'b',
        feedGarra: 'g',
        feedMuneca: 'a'
    }
    
    if feed_id in feed_to_char:
        arduino.write(bytes(f'{feed_to_char[feed_id]}\n', 'utf-8'))
        time.sleep(0.1)
    
    angle_map = {
        '0': '0', '10': '1', '20': '2', '30': '3', '40': '4',
        '50': '5', '60': '6', '70': '7', '80': '8', '90': '9',
        '100': 'A', '110': 'B', '120': 'C', '130': 'D', '140': 'e',
        '150': 'F', '160': 'G', '170': 'H', '180': 'I', '190': 'J',
        '200': 'K', '210': 'L', '220': 'M', '230': 'N', '240': 'O',
        '250': 'P', '255': 'q'
    }

    if payload in angle_map:
        arduino.write(bytes(f'{angle_map[payload]}\n', 'utf-8'))

try:
    client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

    # Setup the callback functions defined above.
    client.on_connect = connected
    client.on_disconnect = disconnected
    client.on_message = message

    # Connect to the Adafruit IO server.
    client.connect()
    client.loop_background()
              
    arduino = serial.Serial(port='COM6', baudrate=9600, timeout=0.1)
    
    while True:    
        mensaje = arduino.readline().decode('utf-8')     # Si se recibe un mensaje del atmega328p
        print(mensaje)
        
        if mensaje == 'Z\n':
            print('Entrando a estado EEPROM\n')
            client.publish(feedState, 2)   # Encender LEDs correspondientes en Adafruit
            
        if mensaje == 'A\n':
            print('Entrando a estado en linea\n')
            client.publish(feedState, 3)
            
        if mensaje == 'B\n':
            print('Entrando a estado Manual\n')
            client.publish(feedState, 1)
       
        time.sleep(3)
        
except KeyboardInterrupt:
    print("Salimos del programa")
    if arduino.is_open:
        arduino.close()
    sys.exit(1)
