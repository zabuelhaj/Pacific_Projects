#!/usr/bin/env python

import numpy as np
import cv2
import matplotlib.pyplot as plt
from picamera import PiCamera
from picamera.array import PiRGBArray
from time import sleep

# Turn camera on to capture video.
camera = PiCamera()
camera.rotation = 180
rawCapture = PiRGBArray(camera)

# Loop through images from the camera. Hit ESC to quit or S to save image.
image_num = 1

# Capture frames from the camera.
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    # Grab the raw NumPy array representing the image.
    image = frame.array

    # Show the frame.
    cv2.imshow("Frame", image)
    k = cv2.waitKey(0) & 0xFF

    # Clear the stream in preparation for the next frame.
    rawCapture.truncate(0)

    if k == 27:                         # ESC key.
        cv2.destroyAllWindows()
        break
    elif k == ord('s'):                 # 's' key saves image.
        cv2.imwrite('/home/pi/ECPE_255/Positives/image%s.jpg' % image_num,image)
        image_num = image_num + 1
        cv2.destroyAllWindows()

# Close the camera.
camera.close()