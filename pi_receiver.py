import serial
import time

ser = serial.Serial('/dev/ttyACM0',9600) #ls /dev/tty* replace with correct ACM connection
ser.baudrate=9600

while True:
	read_serial=ser.readline()
	print(read_serial)
