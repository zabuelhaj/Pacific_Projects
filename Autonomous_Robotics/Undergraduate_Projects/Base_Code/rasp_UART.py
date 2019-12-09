 import serial
 import time
 import struct

 port = serial.Serial(
     port='/dev/ttyS0',
     baudrate=9600,
     parity=serial.PARITY_NONE,
     stopbits=serial.STOPBITS_ONE,
     bytesize=serial.EIGHTBITS,
     xonxoff=serial.XOFF,
     rtscts=False,
     dsrdtr=False
 )

 ##########                                                                      ##########
 ##########                      FUNCTIONS                               ##########
 ##########                                                                      ##########

 def send_command_type1(command_no, parameter):
     print("Sending initial start byte...")
     port.write(struct.pack('>B', 170))

     print("Sending command and parameter...")
     port.write(struct.pack('>B', command_no))
     port.write(struct.pack('>B', parameter))

     print ("Closing line...")
     port.write(struct.pack('>B', 85))
 
     print("Waiting for FF")
     while(ord(port.read())!=255):
         pass

def send_command_type2(command_no):
    print("Sending initial start byte...")
    port.write(struct.pack('>B', 170))

    print("Sending command...")
    port.write(struct.pack('>B', command_no))

    print ("Closing line...")
    port.write(struct.pack('>B', 85))

    print("Waiting for data")
    while(ord(port.read())!=170):
        pass

    data = ord(port.read())

    print("Waiting for close")
    while(ord(port.read())!=85):
        pass

    print("Recieved")
    print(data)

    return data


##########                                                                      ##########
##########                              MAIN                            ##########
##########                                                                      ##########

#init port
#port.open()
port.isOpen()

print("Initializing the line...")
port.write(bytes(0x00))

while(1):
        command_no = int(raw_input("Enter command number"), 12)
        if command_no  < 5:
                parameter = int(raw_input("Enter parameter"), 12)
                value = send_command_type1(command_no, parameter)
        else:
                value = send_command_type2(command_no)
        print("Got this value:")
        print(value)

print("Closing port")
port.close()

