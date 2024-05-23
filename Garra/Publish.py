# Import standard python modules.
import sys
import time
import random
import serial


# This example uses the MQTTClient instead of the REST client
from Adafruit_IO import MQTTClient

# holds the count for the feed
global run_count

ADAFRUIT_IO_USERNAME = "rudy_04"
ADAFRUIT_IO_KEY      = "aio_bDCo25V3NA0ErGSDRyAwGz1KKLQB"

# Set to the ID of the feed to subscribe to for updates.
#feedContador = 'contador'
feedservo1 = 'base'
feedservo2 = 'brazo'
feedservo3 = 'garra'
feedservo4 = 'muneca'



mensaje = ''

# Define callback functions which will be called when certain events happen.
def connected(client):
    """Connected function will be called when the client is connected to
    Adafruit IO.This is a good place to subscribe to feed changes.  The client
    parameter passed to this function is the Adafruit IO MQTT client so you
    can make calls against it easily.
    """
    # Subscribe to changes on a feed named Counter.
    #print('Subscribing to Feed {0} and {1}'.format(feedLed, feedContador))
    client.subscribe(feedservo1)
    client.subscribe(feedservo2)
    client.subscribe(feedservo3)
    client.subscribe(feedservo4)
    

    
    
    print('Waiting for feed data...')

def disconnected(client):
    """Disconnected function will be called when the client disconnects."""
    sys.exit(1)

def message(client, feed_id, payload):
    """Message function will be called when a subscribed feed has a new value.
    The feed_id parameter identifies the feed, and the payload parameter has
    the new value.
    """
    print('Feed {0} received new value: {1}'.format(feed_id, payload))
    if(feed_id == feedservo1 or feed_id == feedservo2 or feed_id == feedservo3 or feed_id == feedservo4):
        if(feed_id == feedservo1):
            arduino.write(bytes('x', 'utf-8'))
            time.sleep(0.1)
            
        if(feed_id == feedservo2):
            arduino.write(bytes('y', 'utf-8'))
            time.sleep(0.1)
            
        if(feed_id == feedservo3):
            arduino.write(bytes('z', 'utf-8'))
            time.sleep(0.1)
            
        if(feed_id == feedservo4):
            arduino.write(bytes('w', 'utf-8'))
            time.sleep(0.1)
            
        if(payload == '0'):
            print('SLIDER VALUE: 0')
            arduino.write(bytes('0', 'utf-8'))
            #arduino.write(bytes('0', 'utf-8'))
                
        if(payload == '10'):
            print('SLIDER VALUE: 10')
            arduino.write(bytes('1', 'utf-8'))
                
        if(payload == '20'):
            print('SLIDER VALUE: 20')
            arduino.write(bytes('2', 'utf-8'))
                
        if(payload == '30'):
            print('SLIDER VALUE: 30')
            arduino.write(bytes('3', 'utf-8'))
                
        if(payload == '40'):
            print('SLIDER VALUE: 40')
            arduino.write(bytes('4', 'utf-8'))
                
        if(payload == '50'):
            print('SLIDER VALUE: 50')
            arduino.write(bytes('5', 'utf-8'))
            
        if(payload == '60'):
            print('SLIDER VALUE: 60')
            arduino.write(bytes('6', 'utf-8'))
                
        if(payload == '70'):
            print('SLIDER VALUE: 70')
            arduino.write(bytes('7', 'utf-8'))
                
        if(payload == '80'):
            print('SLIDER VALUE: 80')
            arduino.write(bytes('8', 'utf-8'))
                
        if(payload == '90'):
            print('SLIDER VALUE: 90')
            arduino.write(bytes('9', 'utf-8'))
                
        if(payload == '100'):
            print('SLIDER VALUE: 100')
            arduino.write(bytes('a', 'utf-8'))
                
        if(payload == '110'):
            print('SLIDER VALUE: 110')
            arduino.write(bytes('b', 'utf-8'))
                
        if(payload == '120'):
            print('SLIDER VALUE: 120')
            arduino.write(bytes('c', 'utf-8'))
                
        if(payload == '130'):
            print('SLIDER VALUE: 130')
            arduino.write(bytes('d', 'utf-8'))
                
        if(payload == '140'):
            print('SLIDER VALUE: 140')
            arduino.write(bytes('e', 'utf-8'))
                
        if(payload == '150'):
            print('SLIDER VALUE: 150')
            arduino.write(bytes('f', 'utf-8'))
                
        if(payload == '160'):
            print('SLIDER VALUE: 160')
            arduino.write(bytes('g', 'utf-8'))
                
        if(payload == '170'):
            print('SLIDER VALUE: 170')
            arduino.write(bytes('h', 'utf-8'))
                
        if(payload == '180'):
            print('SLIDER VALUE: 180')
            arduino.write(bytes('i', 'utf-8'))
                
        if(payload == '190'):
            print('SLIDER VALUE: 190')
            arduino.write(bytes('j', 'utf-8'))
                
        if(payload == '200'):
            print('SLIDER VALUE: 200')
            arduino.write(bytes('k', 'utf-8'))
                
        if(payload == '210'):
            print('SLIDER VALUE: 210')
            arduino.write(bytes('l', 'utf-8'))
                
        if(payload == '220'):
            print('SLIDER VALUE: 220')
            arduino.write(bytes('m', 'utf-8'))
                
        if(payload == '230'):
            print('SLIDER VALUE: 230')
            arduino.write(bytes('n', 'utf-8'))
                
        if(payload == '240'):
            print('SLIDER VALUE: 240')
            arduino.write(bytes('o', 'utf-8'))
                
        if(payload == '250'):
            print('SLIDER VALUE: 250')
            arduino.write(bytes('p', 'utf-8'))
                
        if(payload == '255'):
            print('SLIDER VALUE: 255')
            arduino.write(bytes('q', 'utf-8'))
            
            
    ##################################################################################################
            
        
    


try:
    client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

    # Setup the callback functions defined above.
    client.on_connect = connected
    client.on_disconnect = disconnected
    client.on_message = message

    # Connect to the Adafruit IO server.
    client.connect()
    client.loop_background()
              
    
    #arduino = serial.Serial(port='/dev/cu.usbserial-110', baudrate =9600, timeout = 0.1)
    arduino = serial.Serial(port='COM6', baudrate =9600, timeout = 0.1)
    
    while True:    
        mensaje = arduino.readline().decode('utf-8')
        if(mensaje == 'LED1\n'):
            print('LED 1\n')
            client.publish(feedLed1, 1)
        if(mensaje == 'LED2\n'):
            print('LED 2\n')
            client.publish(feedLed2, 2)
        time.sleep(3)
        
        
except KeyboardInterrupt:
    print("Salimos del programa")
    if arduino.is_open:
        arduino.close()
    sys.exit(1)