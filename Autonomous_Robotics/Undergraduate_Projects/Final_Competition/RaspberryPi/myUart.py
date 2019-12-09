import serial
import time
from time import sleep
import struct
import math
import numpy
import RPi.GPIO as GPIO

toggle = 1
precision_multiplier = 10
delays = .03

port = serial.Serial(
    port='/dev/ttyS0',
    baudrate=9600,
)

##########                      FUNCTIONS                               ##########

def read_const():
    port.close()
    port.open()
    time.sleep(delays)
    while(1):
         print ord(port.read())
    port.close()

def print_functions():
    print "0: brake"
    print "1: drive forward"
    print "2: drive backward"
    print "3: turn right"
    print "4: turn left"
    print "5: read both bumpers"
    print "6: read right IR"
    print "7: read center IR"
    print "8: read left IR"
    print "9: get obstacle distance"
    print "10: get pose"
    print "11: set speed"
    print "12: toggle obstacle avoidance"

def send_command_type1(command_no, parameter):
    port.close()
    port.open()
    time.sleep(delays)
    #print("Sending initial start byte...")
    port.write(struct.pack('>B', 170))
    #print "[TYPE1] Parameter:", parameter
    #print("Sending command and parameter...")
    port.write(struct.pack('>B', command_no))
    port.write(struct.pack('>B', parameter))

    #print ("[TYPE1] Closing line...")
    port.write(struct.pack('>B', 85))
    
    #print("[TYPE1] Waiting for item in buffer")
    no_bytes = port.in_waiting
    while(1):
        while(no_bytes<1):
            no_bytes = port.in_waiting
        #print "[TYPE1] Checking for 0x55"
        if ord(port.read()) ==85:
            break
    
    port.close()

def send_command_type2(command_no):
    port.close()
    port.open()
    time.sleep(delays)
    global toggle
    if command_no==12:
        #print "Toggling Low Level Object"
        if toggle:
            toggle = 0
        else:
            toggle = 1
    #print("Sending initial start byte...")
    port.write(struct.pack('>B', 170))

    #print("Sending command...")
    port.write(struct.pack('>B', command_no))

    #print ("Closing line...")
    port.write(struct.pack('>B', 85))

    data = port.read(3)
    #print data
    #print ord(data[0]), ord(data[1]), ord(data[2])
    if ord(data[0]) == 170 and ord(data[2]) == 85:
        data_val = ord(data[1])
    else:
        data_val = send_command_type2(command_no)
    
    #print("Recieved")
    #print(data)
    port.close()

    return data_val

def global_pose(command_no):
    port.close()
    port.open()

    time.sleep(delays)
    data = [0, 0, 0]
    #print("Sending initial start byte...")
    port.write(struct.pack('>B', 170))

    #print("Sending command...")
    port.write(struct.pack('>B', command_no))

    #print ("Closing line...")
    port.write(struct.pack('>B', 85))
    data = []
    temp = ord(port.read())
    #print "FIRST BYTES"
    #print temp
    while temp!= 170:
        temp = ord(port.read())
        #print temp
        pass
    #print("[GLOBAL POSE] Waiting for close")
    tem = ord(port.read())
    #print "MAKE WAY FOR LORD FIRST DATA PIECE"
    #print tem
    while tem!= 85:
        data.append(tem)
        tem = ord(port.read())

    ist=len(data)
    #print data
    if ist!=6:
        port.close()
        time.sleep(delays)
        port.open()
        value = global_pose(0xA)
    else:
        data3 = [data[ist-6], data[ist-5], data[ist-4], data[ist-3], data[ist-2], data[ist-1]]

        data_val = [0, 0, 0]
        if(data3[3]):
            data_val[0] = float(-1*data3[0])
        else:
            data_val[0] = float(data3[0])
        if(data3[4]):
            data_val[1] = float(-1*data3[1])
        else:
            data_val[1] = float(data3[1])
        if(data3[5]):
            data_val[2] = float(-1*data3[2])/precision_multiplier
        else:
            data_val[2] = float(data3[2])/precision_multiplier

        #print "[GLOBAL POSE] Finished global pose"
        value = [data_val[0], data_val[1], data_val[2]]
    port.close()
    return value

def pulse_int_pin():
    GPIO.output(5, True)
    time.sleep(.1)
    GPIO.output(5, False)

