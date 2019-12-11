#!/usr/bin/python

import cv2
from time import sleep
import numpy as np

# Open each image in the Negatives/ directory and verify they're all there.
for i in range(178,400):
    img = cv2.imread('/home/pi/ECPE_255/Negatives/image%s.jpg' % i, 0)
    cv2.imshow("Image", img)
    cv2.waitKey(1)                  # Required to display image.
    print("Showing Image: ",i)
    sleep(2)
    cv2.destroyAllWindows()
    sleep(2)
