#!/usr/bin/python

''' This program will do an object detection for Chibi Rem from Re:Zero.

    Required:   Python 3, numpy, robotControl
    Group:      Zachariah Abuelahaj and Norlan Prudente
'''
# Libraries needed for robot and camera
import robotControl
import serial
import math
from time import sleep
from picamera import PiCamera
from picamera.array import PiRGBArray

# Libraries needed for OpenCV
import numpy as np
import cv2

# Libraries needed for multiprocessing.
import multiprocessing as mp

# Open the serial port.
ser = serial.Serial("/dev/serial0",115200)
print("Making connection to Microcontroller.")

def driveRobotForward():
	robotControl.moveRobotXY(10,0,ser)
	result = robotControl.readResponse(ser)
	sleep(0.5)

def is_empty(any_structure):
    if np.any(any_structure):
        return 1                # If not empty return 1.
    else:
        return 0

def main():
    
    # get the haar xml files for anime
    faceCascade = cv2.CascadeClassifier('cascade.xml')

    # Initialize flag to switch between started and stopped processes.
    startStop = 0

    # turn camera on to capture video
    camera = PiCamera()
    camera.framerate = 60
    camera.rotation = 180
    camera.resolution = (1920, 1080)	# Force resolution.
    rawCapture = PiRGBArray(camera)
    sleep(2)                            # Let camera warm-up.

    counter = 0

    # while camera is on
    for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
        # read the camera
        img = frame.array

        # covert it to grayscale
        grayscaled = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        # get the waifu
        faces = faceCascade.detectMultiScale(grayscaled, 3, 3)

        if (is_empty(faces) == 1 and startStop == 0):			# If faces has content, then check alignment.
            counter = counter + 1
            print(counter)
            # Create the process to start driving wheels.
            if (faces[0][0] + faces[0][2] < 700):
            	# Turn Left 15 degrees.
            	print("Turning Left: ", faces[0][0] + faces[0][2])
            	robotControl.rotateRobot(ser, 15*math.pi/180)
            	result = robotControl.readResponse(ser)
            	sleep(0.5)
            elif (faces[0][0] + faces[0][2] > 1300):
            	# Turn Right 15 degrees.
            	print("Turning Right: ", faces[0][0] + faces[0][2])
            	robotControl.rotateRobot(ser, 195*math.pi/180)
            	result = robotControl.readResponse(ser)
            	sleep(0.5)
            else:
            	startStop = 1           # Started
            	print("Driving a tiny bit: ", faces[0][0] + faces[0][2])
            	process = mp.Process(target=driveRobotForward)
            	process.start()
        elif (is_empty(faces) == 0 and startStop == 1):
            startStop = 0           	# Stop it.
            process.join()              # Might need to terminate instead of join()
            process.terminate()

        # Code below needs to be removed for ssh-only applications.

        # draw rectangle for best girl
        # for (x,y,w,h) in faces:
        #     # Instead of drawing the rectangle, maybe drive forward here???
        #     cv2.rectangle(img,(x,y),(x+w,y+h),(0,255,255),5)

        # show the image with rectangles
        # cv2.imshow('img', cv2.flip(img, 1))
        # wait for user to press the escape key to exit
        # k = cv2.waitKey(10) & 0xff

        # Clear the stream in preparation for the next frame. NEED to keep this for ssh-only applications.
        rawCapture.truncate(0)

        # if k == 27:
        #     break

    # close the camera
    camera.close()
    # Close the serial port.
    ser.close()
    # close all the windows being used for this program. Remove for ssh-only applications.
    # cv2.destroyAllWindows()

if __name__ == '__main__':
    main()
