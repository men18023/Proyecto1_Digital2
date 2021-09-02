from Adafruit_IO import Client, RequestError, Feed
from time import sleep
import serial
import time

ser = serial.Serial('COM2', baudrate = 9600, timeout = 3)
#time.sleep(1)

ADAFRUIT_IO_KEY = "aio_zQJl720vheuaqKVRTpIzo1XMfb8J"
ADAFRUIT_IO_USERNAME = "men18023"
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

while 1:
	ser.write(b'a')
	data1 = ser.readline(2).decode("ascii")
	data2 = ser.readline(1).decode("ascii")
	data3 = ser.readline(3).decode("ascii")
	space = ser.readline(2)
	#data3 = ser.readline().decode("ascii")
	print(data1)
	print(data2)
	print(data3)
	#print(space)
	print("-----")
	Sensor01_feed = aio.feeds('s1p1')
	aio.send_data(Sensor01_feed.key, data1)
	#Sensor01_feed = aio.feeds('s2p1')
	#aio.send_data(Sensor01_feed.key, data2)
	#Sensor01_feed = aio.feeds('s3p1')
	#aio.send_data(Sensor01_feed.key, data3)
	
