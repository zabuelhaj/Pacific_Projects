import serial
import time
import struct
import math
import numpy

precision_multiplier = 10

port = serial.Serial(
    port='/dev/ttyS0',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    xonxoff=serial.XOFF,
    rtscts=False,
    dsrdtr=False
)

##########                                                                      ##########
##########                      FUNCTIONS                               ##########
##########                                                                      ##########

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
    print("Sending initial start byte...")
    port.write(struct.pack('>B', 170))

    print("Sending command and parameter...")
    port.write(struct.pack('>B', command_no))
    port.write(struct.pack('>B', parameter))

    print ("Closing line...")
    port.write(struct.pack('>B', 85))
 
    print("Waiting for FF")
    while(ord(port.read())!=255):
        pass

def send_command_type2(command_no):
    print("Sending initial start byte...")
    port.write(struct.pack('>B', 170))

    print("Sending command...")
    port.write(struct.pack('>B', command_no))

    print ("Closing line...")
    port.write(struct.pack('>B', 85))

    print("Waiting for data")
    while(ord(port.read())!=170):
        pass

    data = ord(port.read())

    print("Waiting for close")
    while(ord(port.read())!=85):
        pass

    print("Recieved")
    print(data)

    return data

def global_pose(command_no):
    data = [0,0,0]
    print("Sending initial start byte...")
    port.write(struct.pack('>B', 170))

    print("Sending command...")
    port.write(struct.pack('>B', command_no))

    print ("Closing line...")
    port.write(struct.pack('>B', 85))

    print("Waiting for data")
    while(ord(port.read())!=170):
        pass

    data[0] = ord(port.read())
    data[1] = ord(port.read())
    data[2] = ord(port.read())

    print("Waiting for close")
    while(ord(port.read())!=85):
        pass
    value = [float(data[0]), float(data[1]), float(data[2])/precision_multiplier ]
    return value

##########                                                                      ##########
##########                              MAIN                            ##########
##########                                                                      ##########

#init port
#port.open()
port.isOpen()

print("Initializing the line...")
port.write(bytes(0x00))

while(1):
    br = int(raw_input("command or coordinate? (1 or 2)"))
    if br == 1:
        print_functions()
        command_no = int(raw_input("Enter command number"), 10)
        #it sends a parameter
        if command_no  < 5 or command_no == 11:
            if command_no == 3 or command_no == 4: 
                parameter = int(float(raw_input("Enter parameter"))*precision_multiplier)
            else:
                parameter = int(raw_input("Enter parameter"), 10)
            value = send_command_type1(command_no, parameter)
        #it receieves a value and sends no parameter
        else:
            print command_no
            if (command_no < 13):
                if (command_no ==10):
                    value = global_pose(0x0A)
                else:
                    value = send_command_type2(command_no)
                    print("Got this value:")
                print(value)
            else:
                print "invalid command number"
                    
    
    elif br == 2:
        dest = [0,0]
        dest[0] = float(raw_input("Enter x coordinate for next location"))
        dest[1] = float(raw_input("Enter y coordinate for next location"))
        #get current global pose
        pose = global_pose(0x0A)
        
        #local vars
        xTan = dest[0] - pose[0]
        yTan = dest[1] - pose[1]
        
        #turn to face the desired destination.
        dir = int(numpy.arctan(yTan/xTan)*precision_multiplier)
        print "Radians:"
        print float(dir)/10
        if dir<0: #turn right
            dir = abs(dir)
            send_command_type1(4, dir)
        else: #turn left
            send_command_type1(3, dir)
        
        #see how far to drive ((x1-x2)^2+(y1-y2)^2)^0.5
        dist = math.sqrt(math.pow(abs(dest[0]-pose[0]),2)+math.pow(abs(dest[1]-pose[1]),2))
        print "Distance:"
        print dist
        #drive straight
        send_command_type1(0x1, int(dist))
    else:
        print "Please enter 1 or 2"

print("Closing port")
port.close()

