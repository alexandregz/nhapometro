#Necesidades del sistema:
#   pip install adafruit-io

import serial
import datetime
import re
import random
import sys
import time
import requests
import json

# Import Adafruit IO MQTT client.
from Adafruit_IO import MQTTClient

# Set to your Adafruit IO key & username below.
ADAFRUIT_IO_KEY      = ''
ADAFRUIT_IO_USERNAME = ''  # See https://accounts.adafruit.com
                                 # to find your username.
FEED = 'dina.nhapometro'

contador = 0
# Create an MQTT client instance.
client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

#INI comandos a enviar al puerto serie
def updateDisplay(objSerie):
    global contador
    objSerie.write("VAL|"+str(contador).zfill(4)+"|\n")

def setColor(objSerie,color):
    objSerie.write("CLR|"+str(color).zfill(2)+"|\n")
    #objSerie.write("CLR|02|\n")

def setEfecto(objSerie):
    objSerie.write("EFT|01|\n")

def apagar(objSerie):
    objSerie.write("OFF|01|\n")

def setConfig(objSerie):
    objSerie.write("SND|0|\n")
    objSerie.write("ZFL|0|\n")
    objSerie.write("BRT|200|\n")
#FIN comandos a enviar al puerto serie

#INI funciones adicionales
def getIniContador():
    global contador

    payload = {'obtener': 'cuenta'}
    respuesta = requests.post("https://xxxxxxxx/xxxxx/xxxxxx.php", data=payload)
    respuesta_json_data = json.loads(respuesta.text)
    contador = int(respuesta_json_data["COUNT"])
#FIN funciones adicionales

#INI Callback functions para el cliente MQTT
def connected(client):
    # Connected function will be called when the client is connected to Adafruit IO.
    # This is a good place to subscribe to feed changes.  The client parameter
    # passed to this function is the Adafruit IO MQTT client so you can make
    # calls against it easily.
    print('Connected to Adafruit IO!  Listening for dina.nhapometro changes...')
    # Subscribe to changes on a feed named dina.nhapometro.
    client.subscribe(FEED)

def disconnected(client):
    # Disconnected function will be called when the client disconnects.
    print('Disconnected from Adafruit IO!')
    client.connect()
    client.loop_background()
    #sys.exit(1)

def message(client, feed_id, payload):
    # Message function will be called when a subscribed feed has a new value.
    # The feed_id parameter identifies the feed, and the payload parameter has
    # the new value.
    print('Feed {0} received new value: {1}'.format(feed_id, payload))
    getIniContador() #actualizar valor

def sendData(client, valor):
    client.publish(FEED, valor)
#FIN Callback functions para el cliente MQTT

def procesar(linea, ser):
    if not linea:
        return

    global contador
    global client

    fechahora = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
    print linea+"  __++__  " + fechahora +"\n"

    pattern = re.compile("^P[0-9][0-9].*")
    if pattern.match(linea):
        contador += 1 #actualizar un incremento al instante
        updateDisplay(ser) #actualizar un incremento al instante
        elemento = linea[1:3]
        payload = {'elemento': 'BTN|0'+str(elemento)+'|'}
        respuesta = requests.post("https://xxxxx/xxxx/xxxxxxx.php", data=payload)
        respuesta_json_data = json.loads(respuesta.text)
        contador = int(respuesta_json_data["COUNT"])
        sendData(client, 'BTN|0'+str(elemento)+'|') #actualizar en Adafruit
        last_update = 0 #forzar update en display



# Setup the callback functions defined above.
client.on_connect    = connected
client.on_disconnect = disconnected
client.on_message    = message

# Connect to the Adafruit IO server.
client.connect()

# The first option is to run a thread in the background so you can continue
# doing things in your program.
client.loop_background()

getIniContador()

last_update = datetime.datetime.now()
last_update_efecto = datetime.datetime.now()
last_update_config = datetime.datetime.now() - datetime.timedelta(seconds=55)
puerto_serie = "/dev/ttyACM0" #Dell Linux
#puerto_serie = "/dev/cu.usbmodem1411" #Mac OSX
with serial.Serial(puerto_serie, 9600, timeout=0.5) as ser:  # open serial port
    while True:
        line = ser.readline()
        procesar(line, ser)             

        if (datetime.datetime.now()-last_update_config).total_seconds() >= 59:
            last_update_config = datetime.datetime.now()
            setConfig(ser)
            setColor(ser,2)

        if (datetime.datetime.now()-last_update).total_seconds() >= 1:
            last_update = datetime.datetime.now()
            if( ( (datetime.datetime.now().hour >= 6) and (datetime.datetime.now().hour <= 17) ) and
                ( (datetime.datetime.now().weekday() >= 0) and (datetime.datetime.now().weekday() <=4) ) ):
                updateDisplay(ser)
            else: # apagar fuera de una jornada laboral
                apagar(ser) 
            #ser.write("VAL|"+str(contador).zfill(4)+"|\n")

        #if (datetime.datetime.now()-last_update_efecto).total_seconds() >= 5:
        #   last_update_efecto = datetime.datetime.now()
        #   setEfecto(ser)

    ser.close()


