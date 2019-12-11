## Import Necessary Packages
import robotControl
import serial
import math
import struct
import time
import numpy as np
import random
from picamera.array import PiRGBArray
from picamera import PiCamera
import cv2
import grabControl as gc  
import commModule as comm

# Authors: Zachariah Abuelhaj, Joe Coria, Trevor Speckman

# Open Serial Port to robot and Communication Port
ser = serial.Serial("/dev/serial0",115200)
radio = comm.connectXBee()
robotControl.changePWMvalues(ser, 135, 155)    

Dist      = 20 # Dist to travel for each check
wait      = .5 # Time to wait between checking if object moved
area_min  = 100000 # minimum area required to begin gripping
# IR Sensor threshold Values (closer than 5 cm)
L_min     = 1000
M_min     = 1000
R_min     = 1000
# Center coordinates of raspberry pi camera module
xc        = 360
yc        = 240
pi = math.pi
# initialize the camera and grab a reference to the raw camera capture
camera            = PiCamera()
camera.resolution = (720,480)
rawCapture        = PiRGBArray(camera, size=(720,480))
camera.rotation   = 180
rawCapture.truncate(0)
#Global constants for the IR sensors.
leftM_prime = 1/(6.38087*10**(-5)); leftQ_prime = 1/(4.86854*10**(-3));
middleM_prime = 1/(6.28678*10**(-5)); middleQ_prime = 1/(2.60554*10**(-3));
rightM_prime = 1/(6.30759*10**(-5)); rightQ_prime = 1/(1.93912*10**(-3));

def Bound(area):
    xc_low = 0
    xc_high = 0
    if area < 5000:
        xc_low = xc-30
        xc_high = xc+30
    elif area > 5000 and area < 75000:
        xc_low = xc - 60
        xc_high = xc + 60
    elif area > 75000:
        xc_low = xc - 80
        xc_high = xc+ 80
        
    return xc_low, xc_high
        

# function for averaging sensor data
def sensor_Average():
    L_tot = 0   
    M_tot = 0
    R_tot = 0
    for i in range(0,10):
        robotControl.getSensors(ser)
        result, msgType = robotControl.readResponse(ser)
        while msgType != 3:
            robotControl.getSensors(ser)
            result, msgType = robotControl.readResponse(ser)
        L_tot = result[0] + L_tot
        M_tot = result[1] + M_tot
        R_tot = result[2] + R_tot
    L_avg = L_tot/10
    M_avg = M_tot/10
    R_avg = R_tot/10

    return L_avg, M_avg, R_avg


# Function for reading sensors and returning distance for grab function
def readIRSensors ():
	robotControl.getSensors(ser)
	result, msgType = robotControl.readResponse(ser)		# Need dummy read for IR sensors.
	time.sleep(0.5)
	left = 0; middle = 0; right = 0;
	for i in range(0, 6):
		robotControl.getSensors(ser)
		result, msgType = robotControl.readResponse(ser)
		time.sleep(0.5)
		left = left + result[0]; middle = middle + result[1]; right = right + result[2];
	left = left/5; middle = middle/5; right = right/5;

	sensors = []
	sensors.append((leftM_prime/(left + leftQ_prime) - 0.42))
	sensors.append((middleM_prime/(middle + middleQ_prime) - 0.42))
	sensors.append((rightM_prime/(right + rightQ_prime) - 0.42))
	
	return sensors

def nothing(x):
    pass

# This function will change the HSV values for whatever we need them to be.
# They default to our green noodle color.
def changeHSV(sel):
    if (sel == 0):
        #Filter values (For our noodle)
        h_high  = 90
        h_lower = 30
        s_high  = 255
        s_lower = 70
        v_high  = 200
        v_lower = 20
    elif (sel == 1):
        #Filter values (For red noodle)
        h_high  = 13
        h_lower = 0
        s_high  = 245
        s_lower = 0
        v_high  = 185
        v_lower = 109
    elif (sel == 2):
        # Filter values for white wall - ZA
        h_high  = 255
        h_lower = 19
        s_high  = 255
        s_lower = 0
        v_high  = 255
        v_lower = 0
    elif (sel == 3):  # filter  values for orange square
        h_high  = 180
        h_lower = 0
        s_high  = 255
        s_lower = 150
        v_high  = 255
        v_lower = 150
    else:										# Probably add in the rest of the important values.
        h_high  = 90
        h_lower = 30
        s_high  = 255
        s_lower = 70
        v_high  = 200
        v_lower = 20

    hsv_min    = np.array([h_lower, s_lower, v_lower])  # Low threshold for ball color
    hsv_max    = np.array([h_high, s_high, v_high])     # High threshold for ball color
    
    return hsv_min, hsv_max

# Camera process for getting centroid and object area
def camera_capture():
    # Take image for processing HSV limits
    temp_x_center = 0
    temp_check_flag = 0
    temp_object_area = 0
    temp_cnts = 0
    temp_y_center = 0
    
    rawCapture.truncate(0)
    camera.capture(rawCapture, format="bgr")
    ref_image = rawCapture.array #Test Image
    ref_image = cv2.cvtColor(ref_image, cv2.COLOR_BGR2HSV)
   
    #Get thresholded image, erode and dilate to remove noise
    thresholded_image = cv2.inRange(ref_image, hsv_min, hsv_max)
    thresholded_image = cv2.erode(thresholded_image, None, iterations = 2)
    thresholded_image = cv2.dilate(thresholded_image, None, iterations = 2)

    # Detect pool noodle contour in the image
    __, temp_cnts, __ = cv2.findContours(thresholded_image, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    # if object found in image find area and centroid (only count if above 500 area)
    if len(temp_cnts) > 0:
        c = max(temp_cnts, key=cv2.contourArea)
        image_moments = cv2.moments(c)
        temp_object_area = image_moments['m00']
        if temp_object_area > 1000:
            temp_x_center = int(image_moments['m10']/temp_object_area)
            temp_y_center = int(image_moments['m01']/temp_object_area)
            x, y, w, h = cv2.boundingRect(c)
            cv2.rectangle(ref_image, (x,y), (x+w,y+h), (0,255,0),2)
            temp_x_center  = int((x+x+w)/2)  
            print("Object found, center x: ", temp_x_center)
            print("Area: ", temp_object_area)
            temp_check_flag = 1
        elif temp_object_area < 1000:
            temp_check_flag  = 0
            temp_x_center    = 0
            temp_object_area = 0
            print("Object Area not bigger than 500")
    else:
        if len(temp_cnts) == 0:
            print("No Object Found while in captre function")
            temp_check_flag = 0
            
    return temp_check_flag, temp_x_center, temp_object_area


def seek_Object():        # Coverage
    comm.readPacket(radio,  3)
    print("No Object Found - begin coverage")
    # Define the flags here, object-in-view flag is already defined below: - ZA
    robOrWall = 0			# 0 = nothing to do. 1 = robot, 2 = wall.
    wallCheck = 0
    # Get sensor data
    [L_avg, M_avg, R_avg] = sensor_Average()
    print("Left Sensor:", L_avg, "Middle Sensor:", M_avg, "Right Sensor:", R_avg)
    # If only left sensor near wall okay, move forward
    if (L_avg > L_min and R_avg < R_min and M_avg < M_min):
        robotControl.moveRobotXY(ser, Dist, 0)
        result, obs = robotControl.readResponse(ser)
        print(obs)
        if(obs >= 1):
            robotControl.rotateRobot(ser, ((math.pi/16)+math.pi))
            result, msgType = robotControl.readResponse(ser)
            print("Object Flagged, Turning right  away from wall")
        print("Moving Forward, Left Wall follow")
    # if only right sensor near wall, move forward
    elif (L_avg < L_min and R_avg > R_min and M_avg < M_min):
        robotControl.moveRobotXY(ser, Dist, 0)
        result, obs = robotControl.readResponse(ser)
        print(obs)
        if(obs >= 1):
            robotControl.rotateRobot(ser, (math.pi/16))
            robotControl.readResponse(ser)
            print("Object Flagged, Turning left  away from wall")
        print("Moving Forward, right wall follow")
    # if along left wall with obstacle in front, go to obstacle avoidance
    elif (L_avg > L_min and R_avg < R_min and M_avg > M_min):
        wallCheck = 1
        print("Going into object Avoidance - Left/Front Blocked")
    # if along right wall with obstacle in front, go to obstacle avoidance
    elif (L_avg < L_min and R_avg > R_min and M_avg > M_min):
        wallCheck= 1
        print("Going into Object Avoidance - Right/Front Blocked")
    # if only middle sensor near wall, move forward
    elif (L_avg < L_min and R_avg > R_min and M_avg < M_min):
        robotControl.moveRobotbackX(ser, 5)
        result, obs = robotControl.readResponse(ser)
        print(obs)
        if(obs >= 1):
            robotControl.rotateRobot(ser, (math.pi/16))
            result, msgType = robotControl.readResponse(ser) 
            print("Object Flagged, Turning away from  object")
        print("Moving backward, dont hit anything")
    # If blocked on all 3 Sides, turn 180 degrees
    elif (L_avg > L_min and R_avg > R_min and M_avg > M_min):
        robotControl.rotateRobot(ser, math.pi)
        result, msgType = robotControl.readResponse(ser)
        print("Turning Around") 
    # if all 3 sides not blocked begin loop until side is found
    elif (L_avg < L_min and R_avg < R_min and M_avg < M_min):
        print("No sensors triggered")
        uflag = 0
        driveDist = 15
        while(uflag == 0):
            # Drive in increasing in size square till a wall or object is found
            robotControl.rotateRobot(ser, math.pi/2)
            result, msgType= robotControl.readResponse(ser)
            time.sleep(wait)
            robotControl.moveRobotXY(ser, driveDist, 0)
            result, msgType = robotControl.readResponse(ser)
            time.sleep(wait)
            driveDist = driveDist + 5
            print("Drive Distance:", driveDist)
            time.sleep(wait)
            print("checking to see if object in view")
            checkFlag, x_center, object_area = camera_capture()
        
            if checkFlag == 1:
                # line up with object, confirm it is object, then set objectFlag to 1
                object_detected = False
                xc_low, xc_high = Bound(object_area)
                while not object_detected:
                    comm.readPacket(radio,  3)
                    if x_center < (xc_low):
                        print ("LEFT!")
                        robotControl.rotateRobot(ser, pi/25.7)  
                        result, msgType = robotControl.readResponse(ser)                       
                    # Object to Left of center, rotate right    
                    elif x_center > (xc_high):
                        print ("RIGHT!")
                        robotControl.rotateRobot(ser,(pi+(pi/25.7)))
                        result, msgType = robotControl.readResponse(ser)                      
                    else:
                        if x_center in range((xc_low),(xc_high)):
                            object_detected = True
                    print("center of object:", x_center, "needs to be:", xc)
                    print("I am in turning loop here - seek object function ")
                    rawCapture.truncate(0)
                    time.sleep(wait)
                    checkFlag, x_center, object_area = camera_capture()
                    xc_low, xc_high = Bound(object_area)
                time.sleep(wait)
                print("Checking to see if object has moved")
                time.sleep(wait)
                checkFlag, xc_temp, temp_area = camera_capture()
                if((x_center-30)<= xc_temp <=(x_center+30) and temp_area >= (0.97*object_area)):
                    objectFlag = 1
                    print("object confirmed - going to object")
                    uflag = 1    # break out of loop
                
            L_avg, M_avg, R_avg = sensor_Average()
            print("Left Sensor:", L_avg, "Middle Sensor:", M_avg, "Right Sensor:", R_avg)
            if (L_avg > L_min or R_avg > R_min):
                uflag = 1
                time.sleep(wait)
                
    # Change the HSV values to look for a white wall:
    if (wallCheck == 1):
        hsv_min, hsv_max = changeHSV(2)
        print("min values are:", hsv_min)
        checkFlag, x_center, object_area = camera_capture()
        if object_area >= 50000:
            # There is a wall infront, so set value to 2
            robOrWall = 2
            print("wall in front confirmed")
            print("Object Area is", object_area)
        # No a wall, is an obstacle so set value to 1
        elif object_area < 50000:
            robOrWall = 2
            print("Wall not confirmed - obstacle in front")
            print("Object Area is:" , object_area)
        # obstacle avoidance program
        if (robOrWall == 1):
            L_avg, M_avg, R_avg = sensor_Average()
            print("Left Sensor: ", L_avg, "Middle Sensor: ", M_avg, "Right Sensor: ", R_avg)
            if (L_avg > L_min and R_avg < R_min):
                print("Going into left side avoidance")
                time.sleep(3)
                robotControl.rotateRobot(ser, ((3*math.pi)/2))
                result, msgType = robotControl.readResponse(ser)
                print("Turning Right.")
                print("Driving forward.")
                robotControl.moveRobotXY(ser, int(Dist/2), 0)
                robotControl.readResponse(ser)
                time.sleep(2)
                robotControl.rotateRobot(ser, (math.pi/2))
                result, msgType = robotControl.readResponse(ser)
                print("Turning Left")
                print("Drive forward.")
                robotControl.moveRobotXY(ser, int(Dist/2), 0)
                robotControl.readResponse(ser)
                time.sleep(2)
                robotControl.rotateRobot(ser, (math.pi/2))
                result, msgType = robotControl.readResponse(ser)
                print("Turning Left")
                print("Drive forward.")
                robotControl.moveRobotXY(ser, int(Dist/2), 0)
                robotControl.readResponse(ser)
                time.sleep(2)
                robotControl.rotateRobot(ser, ((3*math.pi)/2))
                result, msgType = robotControl.readResponse(ser)
                print("Turning Right.")
            elif (L_avg < L_min and R_avg > R_min):
                print("Going into right side avoidance")
                time.sleep(3)
                robotControl.rotateRobot(ser, (math.pi/2))
                result, msgType = robotControl.readResponse(ser)
                print("Turning Left")
                print("Drive forward.")
                robotControl.moveRobotXY(ser, int(Dist/2), 0)
                robotControl.readResponse(ser)
                time.sleep(2)
                robotControl.rotateRobot(ser, ((3*math.pi)/2))
                result, msgType = robotControl.readResponse(ser)
                print("Turning Right.")
                print("Drive forward.")
                robotControl.moveRobotXY(ser, int(Dist/2), 0)
                robotControl.readResponse(ser)
                time.sleep(2)
                robotControl.rotateRobot(ser, ((3*math.pi)/2))
                result, msgType = robotControl.readResponse(ser)
                print("Turning Right.")
                print("Drive forward.")
                robotControl.moveRobotXY(ser, int(Dist/2), 0)
                robotControl.readResponse(ser)
                time.sleep(2)
                robotControl.rotateRobot(ser, (math.pi/2))
                result, msgType = robotControl.readResponse(ser)
                print("Turning Left")
            else:
                print("Why are you here?")
        # Program if wall in front
        if (robOrWall == 2):
            L_avg, M_avg, R_avg = sensor_Average()
            print("Left Sensor: ", L_avg, "Middle Sensor: ", M_avg, "Right Sensor: ", R_avg)
            if (L_avg > L_min and R_avg < R_min):
                print("Making Right Turn")
                robotControl.rotateRobot(ser, ((3*math.pi)/2))
                robotControl.readResponse(ser)
            elif (L_avg < L_min and R_avg > R_min):
                print("Making left  Turn")
                robotControl.rotateRobot(ser, ((3*math.pi)/2))
                robotControl.readResponse(ser)
        wallCheck        = 0 # reset Flag
        robOrWall        = 0 # reset flag
        hsv_min, hsv_max = changeHSV(0) # reset hsv

# Algorithm for returning the object back to the goal
def return_Object():
    print("Going into return object function")
    hsv_min, hsv_max = changeHSV(3)       # set hsv values to orange goal color
    goalFlag = 0
    while(goalFlag == 0):
        # driving to object until sensor value is  max
        print("Beginning to drive to object")
        robotControl.moveRobotObs(ser)
        result, obs = robotControl.readResponse(ser)
        time.sleep(wait)
        #print(result)
        # Once sensor is maxed, check to see if object area is correct
        if(obs >= 1):
            checkFlag, x_center, temp_area = camera_capture() 
            if checkFlag == 1:
                if temp_area >= area_min:
                    print("Made it to the Goal! Team Wall-E Rules!!")
                    goalFlag = 1
                # If sensor caused by obstacle
                elif temp_area < area_min:
                    print("Object area is:", temp_area, "Object Area needs to be:", area_min)
                    print("Going into object avoidance")
                    #seek_object()
                    rotate_Sequence()
                    checkFlag, x_center, object_area = camera_capture()
                    # Line up center of contour with center of robot
                    # Object to Right of center, Rotate left
                    if checkFlag == 1:
                        print("Checking for difference in x")
                        value = x_center - xc
                        print("Difference from center:", value)
                        object_detected = False
                        while not object_detected:
                            if x_center < (xc_low):
                                print ("LEFT!")
                                robotControl.rotateRobot(ser, pi/25.7)  
                                result, msgType = robotControl.readResponse(ser)                               
                            # Object to Left of center, rotate right    
                            elif x_center > (xc_high):
                                print ("RIGHT!")
                                robotControl.rotateRobot(ser,(pi+(pi/25.7)))
                                result, msgType = robotControl.readResponse(ser)                                
                            else:
                                if x_center in range((xc_low),(xc_high)):
                                    object_detected = True
                            print("center of object:", x_center, "needs to be:", xc)
                            print("I am in turning loop here - return object block")
                            rawCapture.truncate(0)
                            time.sleep(wait)
                            checkFlag, x_center, object_area = camera_capture()
                            xc_low, xc_high = Bound(object_area)
                        print("object lined up")
                    elif checkFlag == 0:
                        rotate_Sequence()
            elif checkFlag == 0:
                rotate_Sequence()
        elif obs == 0:
            rotate_Sequence()

# Function for rotating to face flag
def rotate_Sequence():
    L_avg, M_avg, R_avg = sensor_Average()
    for j in range(0,16):
        comm.readPacket(radio,  3)
        if L_avg > R_avg:
            angle = (math.pi/16)
        elif L_avg < R_avg:
                angle = (math.pi/8) + math.pi
        robotControl.rotateRobot(ser, angle)
        result, msgType = robotControl.readResponse(ser)
        checkFlag, x_center, object_area = camera_capture()
        if checkFlag == 1:
            # Line up robot with goal
            print("Checking for difference in x - rotate sequence block")
            value = x_center - xc
            print("Difference from center:", value)
            object_detected = False
            while not object_detected:
                comm.readPacket(radio,  3)
                xc_low, xc_high = Bound(object_area)
                if x_center < (xc_low):
                    print ("LEFT!")
                    robotControl.rotateRobot(ser, pi/30)  
                    result, msgType = robotControl.readResponse(ser)      
                # Object to Left of center, rotate right    
                elif x_center > (xc_high):
                    print ("RIGHT!")
                    robotControl.rotateRobot(ser,(pi+(pi/30)))
                    result, msgType = robotControl.readResponse(ser)            
                else:
                    if x_center in range((xc_low),(xc_high)):
                        object_detected = True
                print("center of object:", x_center, "needs to be:", xc)
                print("I am in turning loop here - rotate sequence block")
                rawCapture.truncate(0)
                time.sleep(wait)
                checkFlag, x_center, object_area = camera_capture()
                xc_low, xc_high = Bound(object_area)
            print("Lined up with goal")
            break


# Main While loop
while(1):
    comm.readPacket(radio,  3)
    objectFlag = 0
    while(objectFlag == 0):
        # Check for package recieved
        comm.readPacket(radio,  3)
        # Take image for processing HSV limits      
        hsv_min, hsv_max = changeHSV(0) # Set HSV values to our noodle
        checkFlag, x_center, object_area = camera_capture()
        print("Check Flag:", checkFlag, "x_center is:", x_center, "object area is:", object_area)
        if checkFlag == 1:
            comm.readPacket(radio,  3)
            # Line up center of contour with center of robot
            print("Checking for difference in x -object Flag block")
            value = x_center - xc
            print("Difference from center:", value)
            # Object to Right of center, Rotate left
            xc_low, xc_high = Bound(object_area)
            object_detected = False
            while not object_detected:
                comm.readPacket(radio,  3)
                if x_center < (xc_low):
                    print ("LEFT!")
                    robotControl.rotateRobot(ser, pi/30)  
                    result, msgType = robotControl.readResponse(ser) 
                # Object to Left of center, rotate right    
                elif x_center > (xc_high):
                    print ("RIGHT!")
                    robotControl.rotateRobot(ser,(pi+(pi/30)))
                    result, msgType = robotControl.readResponse(ser)  
                else:
                    if x_center in range((xc_low),(xc_high)):
                        object_detected = True
                print("center of object:", x_center, "needs to be:", xc)
                print("I am in turning loop here - object 0 block")
                rawCapture.truncate(0)
                time.sleep(wait)
                checkFlag, x_center, object_area = camera_capture()
                xc_low, xc_high = Bound(object_area)
            rawCapture.truncate(0)
            print("object lined up - object 0 block")
            
            # once the robot has turned retake photo to get new center and area
            checkFlag, x_center, object_area = camera_capture()
            print("Check Flag:", checkFlag, "x_center is:", x_center, "object area is:", object_area)        
            
            # Pause and then check if centroid of object has moved
            time.sleep(wait) 
            comm.readPacket(radio,  3)
            print("Checking to see if object has moved")
            time.sleep(wait)
            checkFlag, xc_temp, temp_area = camera_capture()
            if((x_center-30)<= xc_temp <=(x_center+30) and temp_area >= (0.97*object_area)):
                objectFlag = 1
                print("object confirmed - going to object")
            elif checkFlag == 0:
                # Coverage
                print("Object no longer in screen - going to coverage")
                seek_Object()
            else:  # Can we just put return here in order to start checking process over again? Or do we need full driving code to move to next position?
                print("Object Not confirmed - going into coverage")
                print("x_center was:", x_center, " x center now:", xc_temp)
                print("Object Area was:", object_area, "object area now:", temp_area)
                time.sleep(wait)
                seek_Object()
    
        elif checkFlag == 0:
            comm.readPacket(radio,  3)
            seek_Object()          
                
    while(objectFlag == 1):
        comm.readPacket(radio,  3)
        print("Beginning to drive to object")
        # driving to object until sensor value is  max
        robotControl.moveRobotObs(ser)
        result, obs = robotControl.readResponse(ser)        
        time.sleep(wait)
        #obsFlag = 0
        gripFlag = 0
        # Once sensor is maxed, check to see if object area is correct
        if(obs >= 1):
            comm.readPacket(radio,  3)
            print(" Flag hit, checking for object")
            hsv_min, hsv_max = changeHSV(0)
            checkFlag, x_center, temp_area = camera_capture() 
            if checkFlag == 1:  
                if temp_area >= area_min:
                    # Line up with center of object to get ready for 180 deg grip motion
                    print("Object Found - Going to Line up with center of object")
                    object_detected = False
                    while not object_detected:
                        comm.readPacket(radio,  3)
                        if x_center < (xc+ 50):
                            print ("LEFT!")
                            robotControl.rotateRobot(ser, pi/30)  
                            result, msgType = robotControl.readResponse(ser)                           
                        # Object to Left of center, rotate right    
                        elif x_center > (xc+175):
                            print ("RIGHT!")
                            robotControl.rotateRobot(ser,(pi+(pi/30)))
                            result, msgType = robotControl.readResponse(ser) 
                        else:
                            if x_center in range((xc+50),(xc+175)):
                                gripFlag = 1
                                object_detected = True
                        print("center of object:", x_center, "needs to be:", xc)
                        print("I am in turning loop here - Lining up with object block")
                        rawCapture.truncate(0)
                        time.sleep(wait)
                        checkFlag, x_center, object_area = camera_capture()
                    
                # If sensor caused by obstacle, confirm object
                elif temp_area < area_min:
                    obsFlag = 0
                    print("Object area is:", temp_area, "Object Area needs to be:", area_min)
                    print("Going into object avoidance")
                    rotate_Sequence()
                    checkFlag, x_center, object_area = camera_capture()
                    # Line up center of contour with center of robot
                    # Object to Right of center, Rotate left
                    if checkFlag == 1:
                        object_detected = False
                        while not object_detected:
                            comm.readPacket(radio,  3)
                            if x_center < (xc_low):
                                print ("LEFT!")
                                robotControl.rotateRobot(ser, pi/25.7)  
                                result, msgType = robotControl.readResponse(ser)                                
                            # Object to Left of center, rotate right    
                            elif x_center > (xc_high):
                                time.sleep(0.1)
                                print ("RIGHT!")
                                robotControl.rotateRobot(ser,(pi+(pi/25.7)))
                                result, msgType = robotControl.readResponse(ser)                               
                            else:
                                if x_center in range((xc_low),(xc_high)):
                                    object_detected = True
                            print("center of object:", x_center, "needs to be:", xc)
                            print("I am in turning loop here - object already flagged block")
                            rawCapture.truncate(0)
                            time.sleep(wait)
                            checkFlag, x_center, object_area = camera_capture()
                            xc_low, xc_high = Bound(object_area)
                    elif checkFlag == 0:
                        rotate_Sequence()
            elif checkFlag == 0:
                rotate_Sequence()
        elif obs == 0:
            rotate_Sequence()
        
        noodleFlag = 0
        if gripFlag == 1 and noodleFlag == 0:
            # Gripping Sequence, should we first check to see if center of image?
            print("Beginning Grip Sequence")
            sensors = readIRSensors()
            arduinoSerialData = serial.Serial('/dev/ttyACM0', 9600)
            print("OwO whats this?")
            gc.getNoodle(ser, arduinoSerialData, (sensors[1]+3))
            arduinoSerialData.close()
            # Send out flag that we have 
            comm.sendPacket(radio, 1, 5)
            print("Package Sent")
            print("Trying to find goal")
            # Begin motion to go back
            # Change HSV values to orange goal
            hsv_min, hsv_max = changeHSV(3)
            # Begin by making small rotations until goal is found
            rotate_Sequence()
            return_Object()
            print("We should be at the goal, fingers crossed")
            break  
           
            
            
        



    
  