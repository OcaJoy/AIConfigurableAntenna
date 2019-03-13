#Import file to use Serial Communications
from serial import Serial
import struct

#Set up Arduio Port Settings
port = "/dev/ttyACM0"
baudrate = 115200

#Create a serial object for Arduino communications
arduino = Serial(port, baudrate)
arduino.flushInput() #Clear input buffer

#Wait for arduino to finish setting up
while arduino.inWaiting()== 0:
    pass

#Exit program if arduino returns with 0
if arduino.read() != b'\x02':
    print("There is a problem with arduino")
    exit()

# MAIN LOOP
while 1:
    
    # Take user input for frequency
        # an input of 0 will home the antenna
    while 1:
        frequency = input("Input Frequency: ")
        try:
            int(frequency)
            break
        except ValueError:
            print("Input a Number Value")
            
    #Send frequency data to arduino
    arduino.write(str.encode(frequency))
    arduino.flushInput()
    
    #Wait for reply from arduino
    while arduino.inWaiting()== 0:
        pass
    
    reply = arduino.read()
    
    if reply == b'\x01':
        print("Antenna Ready")
    elif reply == b'\x02':
        print("Antenna Homed")
    elif reply == b'\x03':
        print("Frequency not in range")
    else:
        #if arduino returns a value of 0 or anything else
        print("There is a problem with arduino")
        exit()
    
        
    
    

