import serial
import time
import State

from State import State

#Initialize State
root = State()

#initialize Serial Information
ser = serial.Serial('/dev/ttyACM1',9600) #ls /dev/tty* replace with correct ACM connection
ser.baudrate=9600


def sendData(root):
    print('sending...')
    ser.write(bytes(b'A'))
    #root.val = 'ard' #set arduino as active
    time.sleep(1)
    
def receiveData(root):
    print('listening...')
    read_serial=ser.readline()
    if(read_serial):
        print('received', read_serial.decode('utf-8')) #decode data and print
        root.val = 'pi' #set pi as active
    time.sleep(1)

while True:
    if(root.val == 'pi'):
        sendData(root)
    else:
        receiveData(root)

