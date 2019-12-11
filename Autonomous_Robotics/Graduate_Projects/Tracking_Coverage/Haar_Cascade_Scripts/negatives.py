#!/usr/bin/python

# Program to take negative images for deep learning.

import math
import robotControl
import serial
import cv2
import numpy as np
from picamera import PiCamera
from time import sleep

# Open the serial port.
ser = serial.Serial("/dev/serial0",115200)

# Set up the camera.
camera = PiCamera()
camera.rotation = 180
camera.resolution = (100, 100)
sleep(5)

# Sleep to warm-up camera and take some photos.
image_num = 500
for i in range(4):
    for j in range(25):
        sleep(0.5)
        camera.capture('/home/pi/ECPE_255/Negatives/image%s.jpg' % image_num)
        image_num = image_num + 1
    image_num = image_num + 1
    robotControl.rotateRobot(ser, math.pi)
    result = robotControl.readResponse(ser)
    sleep(0.5)

# Open each image and set to grayscale.
for k in range(image_num):
    img = cv2.imread('/home/pi/ECPE_255/Negatives/image%s.jpg' % (k+1),0)
    cv2.imwrite('/home/pi/ECPE_255/Negatives/image%s.jpg' % (k+1), img)

camera.close()

print("Done!")
