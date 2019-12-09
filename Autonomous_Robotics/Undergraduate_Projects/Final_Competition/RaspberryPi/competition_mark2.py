
#Libraries
import RPi.GPIO as GPIO
import time
import myUart
import myUltraSonic
import numpy

#2 foot threshold for corridor identification 
distance_threshold = 60

if __name__ == '__main__':
    dist = [0,0,0,0]
    myUltraSonic.initGPIO()
    xDest = -93
    yDest = -62
    flag = 0
    pose =[0,0,0]
    try:
        while True:
            #pose = myUart.global_pose(0xA)
            print "POSE:", pose
            #set drive straight
            myUart.send_command_type1(1, 250)
            if flag==2:
                time.sleep(.3)
                flag = 0
            if flag == 1:
                time.sleep(.5)
            #Movement determination
            while (5):
                dist = myUltraSonic.scan_US()
                print dist[1] 
                if dist[1] < distance_threshold:
                    print "HERE"
                    break
            options = [0,0,0,0]
            #save off the sensors which are returning valid options
            for i in range(0, 4):
                if dist[i] > distance_threshold:
                    options[i] = i+1
            choices = [0, 0, 0, 0]
            print "OPTIONS:", options
            if 1 in options:
                choices[0]=(pose[2] + 3.14/2)
            if 2 in options:
                choices[1]=(pose[2])
            if 3 in options:
                choices[2]=(pose[2] - 3.14/2)
            if 4 in options:
                choices[3]=(pose[2] + 3.14)
            
            print "Choices:", choices

            max_difference = 3.14
            max_i = i
            xTan = xDest - pose[0]
            yTan = yDest - pose[1]
            theta_dest = abs(numpy.arctan2(yTan, xTan)) - pose[2]/10
            print "Theta Dest:", theta_dest
            #find the valid option which results in the nearest match to theta dest
            for i in range(0, 4):
                difference = theta_dest - choices[i]
                difference = (difference+3.14) % (2*3.14) - 3.14
                max_differencemax_diference = 5
                if difference < max_difference and options[i] != 0:
                    #if there is no other option but back then go backwards
                    print i, options
                    if i == 3:
                        if options[0]==0 and options[1]==0 and options[2]==0:
                            max_difference = difference
                            max_i = i
                            print "CASE"
                    else:
                        max_difference = difference
                        max_i = i
            print "I:", max_i
            
            if options[2]!=0:
                max_i = 2
            elif options[1]!=0:
                max_i = 1
            elif options[0]!=0:
                max_i = 0
            else:
                max_i = 3
            
            #perform direction change down a corridor
            if max_i != 1:
                if max_i == 0:
                    myUart.send_command_type1(4, 3.14/2*10)
                elif max_i == 2:
                    myUart.send_command_type1(3, 3.14/2*10)
                elif max_i == 3:
                    myUart.send_command_type1(4, 3.14*10)
            #perform corridor following
            else:
                if 1 == 0:
                    dist[0]=5
                else:
                    if dist[0] < .5*dist[2]:
                        print "turning right"
                        flag = 2
                        myUart.pulse_int_pin()
                        myUart.send_command_type1(3,4)
                        
                    elif dist[2] <  .5*dist[0]:
                        print "turning left"
                        flag = 2
                        myUart.pulse_int_pin()
                        myUart.send_command_type1(4,4)
                        
                    elif dist[0] < .7*dist[2]:
                        print "turn right a little"
                        flag = 1
                        myUart.pulse_int_pin()
                        myUart.send_command_type1(3, 2)
                        
                    elif dist[2] < .7*dist[0]:
                       print "turn left a little"
                       flag = 1
                       myUart.pulse_int_pin()
                       myUart.send_command_type1(4, 2)
 
    # Reset by pressing CTRL + C
    except KeyboardInterrupt:
        print("Measurement stopped by User")
        GPIO.cleanup()
