# Libraries needed for robot
import robotControl
import serial
import math as m
from time import sleep

# Author: Zachariah Abuelhaj

# Notes:
# Put in turn angle based off of triggered sensor.

def getNoodle (ser, arduinoSerialData, sense):

	turnFlag = 0

	print("Need to backup by ", sense, "cm")
	robotControl.moveRobotBackX(ser, int(sense))
	result = robotControl.readResponse(ser)
	sleep(0.5)

	if (turnFlag == 0):
		print("Going to turn 180 deg.")
		robotControl.rotateRobot(ser, m.pi)
		result = robotControl.readResponse(ser)
		sleep(0.5)
	elif (turnFlag == 1):
		print("Going to turn left 180 degrees.")
		robotControl.rotateRobot(ser, 7*m.pi/4)
		result = robotControl.readResponse(ser)
		sleep(0.5)
	else:
		print("Going to turn right 180 degrees.")
		robotControl.rotateRobot(ser, 160*m.pi/180)
		result = robotControl.readResponse(ser)
		sleep(0.5)

	# travel = 12 # cm

	print("Grabbing noodle.")
	robotControl.moveRobotBackX(ser, int(2*sense))
	result = robotControl.readResponse(ser)
	sleep(0.5)

	# Grab the noodle with Arduino:
	print("Closing the clarm.")
	arduinoSerialData.write(b'3')
	sleep(2)

	print("ET Going Home.")
	robotControl.moveRobotXY(ser, 60, 0)
	result = robotControl.readResponse(ser)
	sleep(0.5)
