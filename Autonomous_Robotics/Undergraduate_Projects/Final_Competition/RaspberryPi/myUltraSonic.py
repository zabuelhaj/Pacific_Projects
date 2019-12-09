import RPi.GPIO as GPIO
import time

#set GPIO Pins
#Left
GPIO_TRIGGER1 = 17
GPIO_ECHO1 = 18
#Middle
GPIO_TRIGGER2 = 27
GPIO_ECHO2 = 22
#Right
GPIO_TRIGGER3 = 23
GPIO_ECHO3 = 24
#Back
GPIO_TRIGGER4 = 20
GPIO_ECHO4 = 26

def initGPIO():
    #GPIO Mode (BOARD / BCM)
    GPIO.setmode(GPIO.BCM) 
    #set GPIO direction (IN / OUT)
    GPIO.setup(GPIO_TRIGGER1, GPIO.OUT)
    GPIO.setup(GPIO_ECHO1, GPIO.IN)
    GPIO.setup(GPIO_TRIGGER2, GPIO.OUT)
    GPIO.setup(GPIO_ECHO2, GPIO.IN)
    GPIO.setup(GPIO_TRIGGER3, GPIO.OUT)
    GPIO.setup(GPIO_ECHO3, GPIO.IN)
    GPIO.setup(GPIO_TRIGGER4, GPIO.OUT)
    GPIO.setup(GPIO_ECHO4, GPIO.IN)
    GPIO.setup(5, GPIO.OUT)
 
def distanceL():
    # set Trigger to HIGH
    GPIO.output(GPIO_TRIGGER1, True)

    # set Trigger after 0.01ms to LOW
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER1, False)

    StartTime = time.time()
    StopTime = time.time()
    begins = time.time()
    timeout = 0
    # save StartTime
    while ((GPIO.input(GPIO_ECHO1) == 0) and timeout == 0):
        StartTime = time.time()
        if (StartTime - begins) > .3:
            print "Timeout occurred, restarting sensor at (start measurement)..."
            timeout = 1
    # save time of arrival
    begins = time.time()
    while ((GPIO.input(GPIO_ECHO1) == 1) and timeout == 0):
        StopTime = time.time()
        if (StopTime - begins) > .5:
            print "Timeout occurred, restarting sensor at (end measurement)..."
            timeout = 1
    if timeout == 0:
        # time difference between start and arrival
        TimeElapsed = StopTime - StartTime
        # multiply with the sonic speed (34300 cm/s)
        # and divide by 2, because there and back
        distance = (TimeElapsed * 34300) / 2
    else:
        distance = distanceL()
    #error check for invalid distances 
    while distance < 2:
        distance = distanceL()
    return distance
    
def distanceM():
    # set Trigger to HIGH
    GPIO.output(GPIO_TRIGGER2, True)

    # set Trigger after 0.01ms to LOW
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER2, False)

    StartTime = time.time()
    StopTime = time.time()
    begins = time.time()
    timeout = 0
    # save StartTime
    while ((GPIO.input(GPIO_ECHO2) == 0) and timeout == 0):
        StartTime = time.time()
        if (StartTime - begins) > .5:
            print "Timeout occurred, restarting sensor at (start measurement)..."
            timeout = 1
    # save time of arrival
    begins = time.time()
    while ((GPIO.input(GPIO_ECHO2) == 1) and timeout == 0):
        StopTime = time.time()
        if (StopTime - begins) > .5:
            print "Timeout occurred, restarting sensor at (end measurement)..."
            timeout = 1
    if timeout == 0:
        # time difference between start and arrival
        TimeElapsed = StopTime - StartTime
        # multiply with the sonic speed (34300 cm/s)
        # and divide by 2, because there and back
        distance = (TimeElapsed * 34300) / 2
    else:
        distance = distanceM()
    #error check for invalid distances 
    while distance < 2:
        distance = distanceM()
    return distance

def distanceR():
    # set Trigger to HIGH
    GPIO.output(GPIO_TRIGGER3, True)

    # set Trigger after 0.01ms to LOW
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER3, False)

    StartTime = time.time()
    StopTime = time.time()
    begins = time.time()
    timeout = 0
    # save StartTime
    while ((GPIO.input(GPIO_ECHO3) == 0) and timeout == 0):
        StartTime = time.time()
        if (StartTime - begins) > .5:
            print "Timeout occurred, restarting sensor at (start measurement)..."
            timeout = 1
    # save time of arrival
    begins = time.time()
    while ((GPIO.input(GPIO_ECHO3) == 1) and timeout == 0):
        StopTime = time.time()
        if (StopTime - begins) > .5:
            print "Timeout occurred, restarting sensor at (end measurement)..."
            timeout = 1
    if timeout == 0:
        # time difference between start and arrival
        TimeElapsed = StopTime - StartTime
        # multiply with the sonic speed (34300 cm/s)
        # and divide by 2, because there and back
        distance = (TimeElapsed * 34300) / 2
    else:
        distance = distanceR()
    #error check for invalid distances 
    while distance < 1:
        distance = distanceR()
    return distance

def distanceB():
    # set Trigger to HIGH
    GPIO.output(GPIO_TRIGGER4, True)

    # set Trigger after 0.01ms to LOW
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER4, False)

    StartTime = time.time()
    StopTime = time.time()
    begins = time.time()
    timeout = 0
    # save StartTime
    while ((GPIO.input(GPIO_ECHO4) == 0) and timeout == 0):
        StartTime = time.time()
        if (StartTime - begins) > .5:
            print "Timeout occurred, restarting sensor at (start measurement)..."
            timeout = 1
    # save time of arrival
    begins = time.time()
    while ((GPIO.input(GPIO_ECHO4) == 1) and timeout == 0):
        StopTime = time.time()
        if (StopTime - begins) > .5:
            print "Timeout occurred, restarting sensor at (end measurement)..."
            timeout = 1
    if timeout == 0:
        # time difference between start and arrival
        TimeElapsed = StopTime - StartTime
        # multiply with the sonic speed (34300 cm/s)
        # and divide by 2, because there and back
        distance = (TimeElapsed * 34300) / 2
    else:
        distance = distanceB()
    #error check for invalid distances 
    while distance < 1:
        distance = distanceB()
    return distance


def normalize(array):
    #sort array such that the last 3 are the worst values
    array.sort()

    mean = (array[0] + array[1] + array[2]) / 3

    #take average of remaining data
    normalized = (array[0] + array[1] + array[2]) / 3

    return normalized
 
def scan_US():
     distL = [0,0,0,0,0,0]
     distM = [0,0,0,0,0,0]
     distR = [0,0,0,0,0,0]
     distB = [0,0,0,0,0,0]
     
     dist = [0,0,0,0]
     #scan all 3 US 6 times and store values
     for i in range(0,6):
         distL[i] = distanceL()
         distM[i] = distanceM()
         distR[i] = distanceR()
         distB[i] = distanceB()
    
     print "DistL", distL
     print "DistM", distM
     print "DistR", distR
     print "DistB", distB
    
     #get single normalized data values
     dist[0] = normalize(distL)
     dist[1] = normalize(distM)
     dist[2] = normalize(distR)
     dist[3] = normalize(distB)
    
     return dist
