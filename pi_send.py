import serial
import time

ser = serial.Serial('/dev/ttyACM0',9600) #ls /dev/tty* replace with correct ACM connection
ser.baudrate=9600

while True:
    ser.write(bytes(b'2'))
    #print(read_serial.decode('utf-8')) #decode data and print
    print('sending...')
    time.sleep(1)