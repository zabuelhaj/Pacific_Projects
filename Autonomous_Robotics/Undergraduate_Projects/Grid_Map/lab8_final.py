import serial
import time
from time import sleep
import struct
import math
import numpy

#currently set to 2 inches
grid_size = 5

precision_multiplier = 10
delays = .03
temp_goal = 3
goal_thresh = 3
toggle = 1
port = serial.Serial(
    port='/dev/ttyS0',
    baudrate=9600,
)

##########                      FUNCTIONS                               ##########

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

def print_map(xpos_val):
    global grid_size
    pos = global_pose(0xA)

    if not xpos_val:
        x_pos = math.floor(pos[0]/grid_size)
    else:
        x_pos = xpos_val
    y_pos = math.floor(abs(pos[1])/grid_size)
    print "POSE:", pos
    map_list = ["-", "-", "-", "-", "-", "-", "-", "-", "-","-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-","-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-", "-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-"]
    print x_pos, y_pos
    map_list[(int(y_pos+10*(9-x_pos)))] = "X"

    for i in range(0, 10):
        print map_list[(i*10):(i*10+10)]
    print""
    return x_pos
##########                              MAIN                            ##########

port.isOpen()

#print("Initializing the line...")
port.write(bytes(0x00))
send_command_type2(0xC)
while 1:
        xflag=0
        yflag=0
        pose = global_pose(0xA)

        not_okay=1
        while(not_okay):
            coordinatex = int(raw_input("Enter X Coordinate (0-9): "))
            coordinatey = int(raw_input("Enter Y Coordinate: (0-9): "))
            if coordinatex >=0 and coordinatex <=9 and coordinatey >=0 and coordinatey<=9:
                not_okay=0
            else:
                print "Invalid Entry"
        print_map(0)
        #get theta needed to turn to face x direction of travel
        tempx = pose[0]
        cur_y = pose[1]
        if tempx > 0:
            tempx = math.ceil(tempx/grid_size)
        #else:
        #   tempx = math.floor(tempx/grid_size)
        tempx = coordinatex - tempx
        if tempx < 0:
            theta_dest = 3.14
        else:
            theta_dest = 0
        print "X theta:", theta_dest
        #turn to go to x direction
        angle = theta_dest - pose[2]/10
        print "X dist:", tempx
        send_command_type1(3, angle*10)
        for i in range(0,  int(abs(tempx))):
            #go to x
            pose = global_pose(0xA)
            mov = 5-math.floor(pose[0] % grid_size)

            send_command_type1(1, mov)
            posx_val = print_map(0)

        pose = global_pose(0xA)
        #get theta needed to turn to face y direction of travel
        tempy = pose[1] - cur_y
        if tempy > 0:
            tempy = math.ceil(tempy/grid_size)
        #else:
        #    tempy = math.floor(tempy/grid_size)
        tempy = coordinatey - tempy
        if tempy < 0:
            theta_dest = 3.14/2
        elif tempy>0:
            theta_dest = -3.14/2
        else:
            theta_dest = 0
        
        #turn to go to y direction
        print "Y theta:", theta_dest
        angle = theta_dest - pose[2]/10
        if angle>0:
            send_command_type1(4, angle*10)
        else:
            angle = abs(angle)
            send_command_type1(3, angle*10)
        #cur_x = pose[0]
        print "Y dist:", tempy
        for i in range(0, int(abs(tempy))):
            #go to y
            pose = global_pose(0xA)
            mov = 5#-math.floor((pose[1]-cur_y) % grid_size)
            print mov
            send_command_type1(1, mov)
            print_map(posx_val)

print("Closing port")
port.close()

