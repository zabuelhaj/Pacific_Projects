'''
Using OpenCV takes a mp4 video and produces a number of images.

Requirements
----
You require OpenCV 3.2 to be installed.

Run
----
Open the main.py and edit the path to the video. Then run:
$ python main.py

Which will produce a folder called data with the images. There will be 2000+ images for example.mp4.
'''
import cv2
import numpy as np
import os

# Playing video from file:
cap = cv2.VideoCapture('rem3.mp4')

try:
    if not os.path.exists('resources/frame'):
        os.makedirs('resources/frame')
except OSError:
    print ('Error: Creating directory of data')

currentFrame = 751
while(True):
    if currentFrame >= 1500:
        break
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Saves image of the current frame in jpg file
    if currentFrame % 1 == 0:
        name = 'resources/frame' + str(int(currentFrame)) + '.jpg'
        print ('Creating...' + name)
        cv2.imwrite(name, frame)

    # To stop duplicate images
    currentFrame += 1

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()
