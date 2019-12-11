#!/usr/bin/python

# Code for robot to play a rigorous game of Simon Says.
# Author: Zachariah Abuelhaj

# For the multiprocessing and queue.
from multiprocessing import Process, Queue

# Libraries needed for robot and random
import robotControl
import serial
import math as m
from time import sleep
import random as r

# Global list, to track all the possible Simons. Access is exclusive to newSimon() function.
robots = [2, 4, 5, 6, 8, 11]			# We are not in this list.

# Random Simon generator:
def getMove():
	ACT = r.randint(1, 3)
	if (ACT == 1):								# Special consideration for move forward commands, 10 - 40 cm.
		PARAM = r.randint(10, 40)
	else:
		PARAM = r.randint(45, 180)
	return (ACT, PARAM)

# Send packet. If timeout on acknowledge read, then resend.
def newSimon(ser):
	# Pick a new Simon. 
	sleep(5)
	print(robots)
	size = len(robots)
	if size == 0:
		return 1						# End flag will be 1 if the list is empty, else return 0.
	index = r.randint(0, size-1)
	nextSimon = robots[index]

	# Construct and send new packet.
	packet = bytearray(); packet.append(0xDA); packet.append(0x07); packet.append(0x4); packet.append(nextSimon); packet.append(0xDB)
	ser.write(packet)

	# Wait for an acknowledge bit. Timeout can occur three times, pick another Simon if no response.
	breakFlag = 0; tryN = 0
	while True:
		if (tryN >= 3):					# On the third try, remove ID from list and get different Simon.
			print("Robot ID ", robots[index], " removed from list.")
			del robots[index]
			endFlag = newSimon(ser)
			return endFlag
		
		datRed = []
		data = int.from_bytes(ser.read(1), 'little')
		if (data == 0xDA):
			print("Reading an acknowledge...")
			datRed.append(data)
			while (data != 0xDB):
				data = int.from_bytes(ser.read(1), 'little')
				datRed.append(data)
			# Check if it was from the correct brobot:
			if (datRed[1] == robots[index]):
				print("Robot acknowledges.")
				breakFlag = 1
		if (breakFlag == 1):
			return 0
		else:
			tryN = tryN + 1
			ser.write(packet)

# Communication process.
def comm(q, simon):
	# Connect to the XBee radio.
	serC = serial.Serial("/dev/ttyUSB0", 57600, timeout = 5)
	print("Connected to device: ", serC.name)

	while True:
		# Check if Simon, if not then read a packet and send an acknowledge.
		if (simon == 1):
			# Randomly decide what to do.
			ACT, PARAM = getMove()

			sleep(2)

			# Construct byte array to broadcast to others.
			packet = bytearray(); packet.append(0xDA); packet.append(0x07); packet.append(ACT); packet.append(PARAM); packet.append(0xDB)
			serC.write(packet)
			print("Sending packet... ", ACT, PARAM)

			# Select a new Simon:
			endFlag = newSimon(serC)
			if (endFlag == 1):
				command = [255, 255]
				q.put(command)
				break
			simon = 0
		else:
			# Read an incoming packet.
			datRed = []
			data = int.from_bytes(serC.read(1), 'little')
			if (data == 0xDA):
				print("Reading a packet...")
				print(data)
				datRed.append(data)
				while (data != 0xDB):
					data = int.from_bytes(serC.read(1), 'little')
					datRed.append(data)
					print(data)

				# Check if this data is for picking a new Simon or not.
				if (datRed[2] == 0x4):
					# Is it us?
					if (datRed[3] == 0x7):
						# Send an acknowledge out.						
						packet = bytearray(); packet.append(0xDA); packet.append(0x07); packet.append(0x05); packet.append(datRed[1]); packet.append(0xDB)
						serC.write(packet)
						print("Picked to be Simon.")

						# Set the Simon flag:
						simon = 1																				# Old Simon ^
				elif (datRed[2] != 0x5):				# Ignore acks from other robots.
					command = [datRed[2], datRed[3]]
					q.put(command)						# Queue-up data for motor control process.

				# Add people who got taken out, but now acknowledge.
				if not (datRed[1] in robots):
					robots.append(datRed[1])

# Motor control process.
def motors(q):
	# Connect to the microcontroller.
	serM = serial.Serial("/dev/serial0", 115200)
	print("Making connection to Microcontroller.")

	# Set the wheel speed.
	robotControl.changePWMvalues(serM, 150, 125)
	sleep(1)

	while True:
		# Read a command from the communication thread.
		command = q.get()
		print("Got a command.")

		# Check the command, act accordingly.
		# Packets in order of [0xDA, SRC, ACT, PARAM, 0xDB];
		# But we will just pass ACT and PARAM through queue.
		ACT = command[0]; PARAM = command[1]

		if (ACT == 0x1):
			robotControl.moveRobotXY(serM, PARAM, 0)
			result, msgType = robotControl.readResponse(serM)
			if (result.obsFlag == 1):
				print("Obstacle detected!")
				robotControl.rotateRobot(serM, m.pi)
				result, msgType = robotControl.readResponse(serM)
			print("Moving forward by ", PARAM)
		elif (ACT == 0x2):
			robotControl.rotateRobot(serM, m.pi*2 - (PARAM*m.pi/180))
			result, msgType = robotControl.readResponse(serM)
			print("Turning left by ", PARAM)
		elif (ACT == 0x3):
			robotControl.rotateRobot(serM, (PARAM*m.pi/180))
			result, msgType = robotControl.readResponse(serM)
			print("Turning right by ", PARAM)
		else:
			print("Robot does not need to move.")
			break
		# We do not care about data concerning the new Simon,
		# or acknowledges in this process.

	serM.close()

# Main function.
def main():
	q = Queue()
	simon = int(input("Enter a 1 for Simon or a 0 for Says "))
	p1 = Process(target=comm, args=(q,simon,))
	p2 = Process(target=motors, args=(q,))
	p1.start()
	p2.start()
	p1.join()
	p2.join()


if __name__=='__main__':
	main()

# Should be able to read IR sensors while robot is driving.
# Microcontorller should be able to take multiple commands at once, as long as they do not interfere.
# i.e. can moveRobotXY() and then readIRSensors(). UART uses FIFO, so have to read twice to get IR sensor values.