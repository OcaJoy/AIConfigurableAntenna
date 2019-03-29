#Import file to use Serial Communications
from serial import Serial

#Set up Arduio Port Settings
port = "/dev/ttyACM0"
baudrate = 115200

#Create a serial object to communicate with Arduino Serially
arduino = Serial(port, baudrate) # Create serial object with specified baudrate and port address
arduino.flush() #Clear input buffer

""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"""
Function: ControlAntenna
Parameter: modeInput - determines the mode the configurable antenna will perform
           legthInput - the desired absolute length of the main and/or reflector antenna
                        (input in centimeter or in hertz depending on the mode selected)
                        
Description: combines the mode instruction and targetted absolute length (that will be calculated in steps)
             into a string and sends it to the Arduino
             Modes depending on the value of instruction Input
             1: homes both main and reflector antenna
             2: controls main and reflector antennas to the required length specified by the inputted frequency
             3: controls main and reflector antennas to the required length specified by the inputted absolute length
             4: controls only the main antenna to the required length specified by the inputted absolute length
             5: controls only the reflector antenna to the required length specified by the inputted absolute length
    
"""
def ControlAntenna (modeInput, lengthInput):    
	#Check if the mode is to home the antennas
	if modeInput == 1:
		lengthInput = 0 # Default the targetted absolute length to 0
    instructionString = str(modeInput) + str(LengthToSteps(lengthInput))
    arduino.write(str.encode(instructionString))
    arduino.flush()
    
"""
Function: DistanceToSteps
Parameter: lengthInput - targetted absolute length 

Description: Converts distance in cm to the number of motor steps required
"""
def LengthToSteps(lengthInput):
    motorSteps = lengthInput * 1
    int(motorSteps)
    return motorSteps

"""
Function: WaitForArduino

Description: Waits for a data that will be sent from the Arduino
"""
def WaitForArduino():
    while arduino.inWaiting == 0:
        pass #do nothing

"""
Function HumanInput

Description: takes user input through keyboard. This function can be removed once human input is not required
"""
def HumanInput():
    human_inputMode = 0
    while 1:
        human_inputModeStringVer = input("Enter Desired Mode: [1, 2, 3, 4, 5]")
        try:
            human_inputMode = int(human_inputModeStringVer)
            break
        except ValueError:
            print("Input number values only")
    
	human_lengthInput = 0
    #human_inputMode = int(human_inputModeStringVer)
    if int(human_inputMode) == 1:
        ControlAntenna(human_inputMode,0)
    else:
        while 1:
            human_lengthInputStringVer = input("Enter Desired Absolute Length: ")
            try:
                huamn_lengthInput = int(human_lengthInputStringVer)
                break
            except ValueError:
                print("Input number values only")
            
        ControlAntenna(human_inputMode, human_lengthInput)
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

#Wait for arduino to finish setting up
WaitForArduino()

#Exit program if arduino is not able to home properly 
if arduino.read(2) != b'02':
    exit() #exits program when arduino is not homed properly

# MAIN LOOP
while 1:
    
    #Take input
    HumanInput() #Remove when human input is no long necessary
    #ControlAntenna(5, 1000) 
    
    #Wait for reply from arduino
    WaitForArduino
    
    #Check Error Log 
    reply = arduino.read(2) # Read the first 2 bytes of the data sent from Arduino  
    if reply == b'00':
        print("ERROR: Communication Error with Arduino")
    elif reply == b'01':
        print("Antenna Ready")
    elif reply == b'02':
        print("Antenna Homed")
    elif reply == b'03':
        print("ERROR: Mistep in belt system of main antenna")
    elif reply == b'04':
        print("ERROR: Mistep in gear system of reflector antennas")
    elif reply == b'05':
        print("ERROR: Frequency not within range")
    elif reply == b'06':
        print("ERROR: Absolute length inputted is not within capability of antennas")
    else:
        #if arduino returns a value of 0 or anything else
        exit()

    
