import serial
import time
import State
from getCoord import getCoord

from State import State


coord_tuples = getCoord()

blockX_str = coord_tuples[0]   #string values of x and y values
blockY_str = coord_tuples[1]

#Initialize State
root = State()

#initialize Serial Information
#ser = serial.Serial('/dev/ttyACM0',9600) #ls /dev/tty* replace with correct ACM connection
ser = serial.Serial('COM4',9600)
ser.baudrate=9600


def sendData(root, msg):
    print('sending...')
    ser.write(msg.encode('utf-8'))
    root.val = 'ard' #set arduino as active
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
        time.sleep(4)
        sendData(root, blockX_str)
        sendData(root, " ")
        sendData(root, blockY_str)
        root.val = "ard"
    else:
        receiveData(root)
