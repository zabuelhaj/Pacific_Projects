import serial
import time
from time import sleep
import struct
import math
import numpy

precision_multiplier = 10
delays = .03
temp_goal = 3
goal_thresh = 3
toggle = 1
port = serial.Serial(
    port='/dev/ttyS0',
    baudrate=9600,
)
'''parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    xonxoff=serial.XOFF,
    timeout = None,
    rtscts=False,
    dsrdtr=False
)
'''

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

def repulsive():
    distance_thresh = 7
    command_no = 9
    IR_distR = send_command_type2(8)
    IR_distL = send_command_type2(7)
    IR_distM = send_command_type2(6)

    print "IR DISTANCE:", IR_distL, IR_distM, IR_distR
    if (IR_distL <= distance_thresh*2.2):
        dirL = -3.1415926 / 3
        magL = (float(IR_distL) - distance_thresh) / float(IR_distL)
    else:
        dirL = 0
        magL = 0
    if (IR_distM <= distance_thresh):
        dirM = 3.1415926 / 2
        magM = (float(IR_distM) - distance_thresh) / float(IR_distM)
    else:
        dirM = 0
        magM = 0
    if (IR_distR <= distance_thresh*2.2):
        dirR = 3.1415926 / 3
        magR = (float(IR_distR) - distance_thresh) / float(IR_distR)
    else:
        dirR = 0
        magR = 0
    dir2 = float(dirL) + float(dirM) + float(dirR)
    mag = float(magL) + float(magM) + float(magR)

    print "repulsivator: dir and mag", dir2, mag

    return [dir2, mag]


def attractionator(yDest, xDest, y, x, th):
    yTan = yDest - y
    xTan = xDest - x

    # First, turn to face the desired destination.
    if (xTan == 0):
        dir1 = 0
    else:
        dir1 = float(float(int(float(numpy.arctan(yTan / xTan) )*10))/10- th)

    # Second, set the magnitude. Because the robot is absolutely attracted to
    # the goal, the magnitude will be max (so 1).
    print "attractionator dir", dir1
    mag = 1;

    return [dir1*.5, mag]


##########                                                                      ##########
##########                              MAIN                            ##########
##########                                                                      ##########

# init port
# port.open()
port.isOpen()

#print("Initializing the line...")
port.write(bytes(0x00))

while 1:
    br = int(raw_input("command, coordinate, or coordinate with vector fields? (1, 2, or 3)"))
    if br == 1:
        print_functions()
        command_no = int(raw_input("Enter command number"), 10)
        # it sends a parameter
        if command_no < 5 or command_no == 11:
            if command_no == 3 or command_no == 4:
                parameter = int(float(raw_input("Enter parameter")) * precision_multiplier)
            else:
                parameter = int(raw_input("Enter parameter"), 10)
            value = send_command_type1(command_no, parameter)
        # it receieves a value and sends no parameter
        else:
            print command_no
            if (command_no < 13):
                if (command_no == 10):
                    value = global_pose(0x0A)
                else:
                    value = send_command_type2(command_no)
                    #print("Got this value:")
                #print(value)
            else:
                print "invalid command number"

    elif br == 2:
        dest = [0, 0]
        dest[0] = float(raw_input("Enter x coordinate for next location"))
        dest[1] = float(raw_input("Enter y coordinate for next location"))
        # get current global pose
        pose = global_pose(0x0A)

        # local vars
        xTan = dest[0] - pose[0]
        yTan = dest[1] - pose[1]
        #print "THETA:"
        print pose[2]
        # turn to face the desired destination.
        dir1 = float(numpy.arctan2(yTan, xTan) *precision_multiplier) - precision_multiplier*pose[2]
        #print "Radians:"
        #print float(dir1) /precision_multiplier
        dir1 = int(dir1)
        if dir1 < 0:  # turn right
            dir1 = abs(dir1)
            send_command_type1(4, dir1)
        else:  # turn left
            send_command_type1(3, dir1)

        # see how far to drive ((x1-x2)^2+(y1-y2)^2)^0.5
        dist = math.sqrt(math.pow(abs(dest[0] - pose[0]), 2) + math.pow(abs(dest[1] - pose[1]), 2))
        #print "Distance:"
        #print dist
        # drive straight
        send_command_type1(0x1, int(dist))

    #POTENTIAL FIELD BUSINESS
    elif br == 3:
        print "VECTOR FIELD OBJECT AVOIDANCE"
        if toggle:
            #toggle the obstacle avoidance
            send_command_type2(12)
        dest = [0, 0]
        dest[0] = float(raw_input("Enter x coordinate for location"))
        dest[1] = float(raw_input("Enter y coordinate for location"))
        # get current global pose
        pose = global_pose(0x0A)

        # local vars
        xTan = dest[0] - pose[0]
        yTan = dest[1] - pose[1]

        goal_thresh = temp_goal + (xTan+yTan)/8
        #print "Threshold:"
        #print goal_thresh
        # turn to face the desired destination.
        dir1 = float(numpy.arctan2(yTan, xTan) *precision_multiplier) - precision_multiplier*pose[2]
        #print "Radians:"
        #print float(dir1) /precision_multiplier
        dir1 = int(dir1)
        
        if dir1 < 0:  # turn right
            dir1 = abs(dir1)
            send_command_type1(4, dir1)
        else:  # turn left
            send_command_type1(3, dir1)
        counter=0
        # check if within goal area
        while (abs(pose[0] - dest[0]) > goal_thresh) or (abs(pose[1] - dest[1]) > goal_thresh):
            vecR = repulsive()
            #print"[VEC] Received vector values front, left, right:", vec
            # get current global pose
            pose = global_pose(0x0A)
            print "POSE: ", pose
            vecA = attractionator(dest[1], dest[0], pose[1], pose[0], pose[2])
            # Combine the vectors to get a direction of travel.
            dir1 = (vecA[0] - vecR[0])
            mag = (vecA[1] - vecR[1])

            raw_input("\npress enter (BEFORE TURN)")
            dir1 = int(dir1*10)
            #if vecR[0] != 0:
            if dir1 < 0:  # turn right
                dir1 = abs(dir1)
                print "Direction (right):", dir1
                counter = counter+1
                send_command_type1(4, dir1)
            elif dir1 > 0: #turn left
                print "Direction (left):", dir1
                send_command_type1(3, dir1)
                counter = counter+1
            if mag<0:
                 mag = abs(mag)
            if mag > 1:
                 mag = 1

            #send_command_type1(0xB, 1)

            #print "DRIVE STRAIGHT"
            send_command_type1(1, 4)
            print "distance from x goal:"
            print abs(pose[0] - dest[0])
            print "distance from y goal:"
            print abs(pose[1] - dest[1])
            print ""
    #BUSINESS COMPLETE
    else:
        print "Please enter 1,2, or 3"

print("Closing port")
port.close()

