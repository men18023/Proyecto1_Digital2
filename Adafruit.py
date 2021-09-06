from Adafruit_IO import Client, RequestError, Feed
from time import sleep
import serial
import time

ser = serial.Serial('COM3', baudrate = 9600)
#time.sleep(1)

ADAFRUIT_IO_KEY = "aio_qcdJ75QbVdIq8p2I9gkPV04HEpRY"
ADAFRUIT_IO_USERNAME = "diego_estrada"
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

while 1:
	ser.close()
	ser.open()
	#Lecturas del serial
	data1 = ser.readline(2).decode('ascii')  #lectura de primeros dos digitos para contador
	data2 = ser.readline(1).decode('ascii')  #lectura de un digito para el on/off
	data3 = ser.readline(3).decode('ascii')  #lectura de 3 digitos finales para temperatura
	#Prints de los valores obtenidos del serial
	print(data1)
	print(data2)
	print(data3)
	print("-----")
	#Comunicación con la herramienta de Adafruit_IO
	Sensor01_feed = aio.feeds('s1p1')       #feed del contador
	aio.send_data(Sensor01_feed.key, data1)
	Sensor02_feed = aio.feeds('s2p1')
	aio.send_data(Sensor02_feed.key, data2) #feed del on/off
	Sensor03_feed = aio.feeds('s3p1')
	aio.send_data(Sensor03_feed.key, data3) #feed de temperatura
	time.sleep(2)   #pausa del while
	
