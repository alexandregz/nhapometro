#Necesidades del sistema:
#	pip install adafruit-io

import serial
import datetime
import re

contador = 0

def procesar(linea):
	if not linea:
		return

	global contador

	fechahora = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
	print linea+"  __++__  " + fechahora +"\n"

	pattern = re.compile("^P[0-9][0-9].*")
	if pattern.match(linea):
		contador += 1

def updateDisplay(objSerie):
	global contador
	objSerie.write("VAL|"+str(contador).zfill(4)+"|\n")

last_update = datetime.datetime.now()
with serial.Serial('/dev/cu.usbmodem1421', 9600, timeout=0.5) as ser:  # open serial port
	while True:
		line = ser.readline()
		procesar(line)

		if (datetime.datetime.now()-last_update).total_seconds() >= 1:
			last_update = datetime.datetime.now()
			updateDisplay(ser)
			#ser.write("VAL|"+str(contador).zfill(4)+"|\n")


	ser.close()