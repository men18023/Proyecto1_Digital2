from Adafruit_IO import Client, RequestError, Feed
from time import sleep
import serial
import time

ser = serial.Serial('COM2', baudrate = 9600)
#time.sleep(1)

ADAFRUIT_IO_KEY = "aio_eOca78chnTouwu1xftAi31ei9Yc9"
ADAFRUIT_IO_USERNAME = "men18023"
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

while 1:
	ser.close()
	ser.open()
	data1 = ser.readline(2).decode('ascii')
	data2 = ser.readline(1).decode('ascii')
	data3 = ser.readline(3).decode('ascii')
	data4 = str(data1)
	#print(data2)
	val = data4.split(',')
	print(data1)
	print(data2)
	print(data3)
	#print(val)
	#data3 = ser.readline(3).decode("ascii")
	#space = ser.readline(2)
	#data3 = ser.readline().decode("ascii")
	#print(space)
	print("-----")
	Sensor01_feed = aio.feeds('s1p1')
	aio.send_data(Sensor01_feed.key, data1)
	Sensor02_feed = aio.feeds('s2p1')
	aio.send_data(Sensor02_feed.key, data2)
	Sensor03_feed = aio.feeds('s3p1')
	aio.send_data(Sensor03_feed.key, data3)
	
