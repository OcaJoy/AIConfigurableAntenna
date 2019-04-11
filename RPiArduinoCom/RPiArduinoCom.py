# Import file to use Serial Communications
from serial import Serial

# Set up Arduio Port Settings
port = "/dev/ttyACM0" #Set serial port address to connect to
baudrate = 115200 #Set the communication baudrate (has to be the same as the Arduino)

# Create a serial object to communicate with Arduino Serially
arduino = Serial(port, baudrate) # Create serial object with specified baudrate and port address

# Clear the output buffer to make sure no unintended data will be sent
arduino.reset_output_buffer()


"""
Function: ControlAntenna                     
Description: takes the desired mode and desired absolute absolute length of the antenna, which will calculated into motor steps, 
             and then combines them into a string to be sent to the Arduino.
             Modes to Choose (1-5):
            
Parameter:  modeInput (int) - determines the mode the configurable antenna will perform
                1: Homes both main and reflector antenna
                2: Moves main and reflector antennas to the desired length specified by the inputted frequency
                3: Moves main and reflector antennas to the desired length specified by the lengthInput
                4: Moves only the main antenna to the desired length specified by the lengthInput
                5: Moves only the main antenna holders to the desired distance specified by the lengthInput
                6: Moves only the reflector antenna to the desired distance specified by lengthInput
            legthInput (int/float) - the desired absolute length of the main and/or reflector antenna
                modeInput = 1: must input a 0
                modeInput = 2: must input a frequency within 320 MHz - 1.6 GHz (integers only)
                modeInput = 3: must input the desired length of the main antenna within 5.6 - 23 centimeters
                modeInput = 4: must input the desired length of The main antenna within 5.6 - 23 centimeters
                modeInput = 5: must input the desired distance of the main antenna holders within 
                modeInput = 6: must input the desired absolute distance of the reflector antennas within 4.51 - 24 centimeters         
Returns: none
"""
def ControlAntenna (modeInput, lengthInput=0):    
    # Check if the mode is to home the antennas
    if modeInput == 1:
        lengthInput = 0 # Default the targetted absolute length to 0
        instructionString = str(modeInput) + str(lengthInput)
    # Check if the mode is to take send a frequency to the Arduino
    elif modeInput == 2:
        instructionString = str(modeInput) + str(lengthInput) #Do not calculate for steps as the Arduino will handle the conversion of frequency to required motor steps 
    # For modes 3-6, the inputted length will be converted into steps using the LengthToSteps() function
    else:
        instructionString = str(modeInput) + str(LengthToSteps(modeInput, lengthInput))
    
    # Send the combined string instruction to the Arduino
    arduino.write(str.encode(instructionString))

    # Wait for the data to finish sending before proceeding
    arduino.flush() 
    
"""
Function: LengthToSteps
Description: This function subtracts the desired absolute length of the antenna with the minimum length of the antenna or the minimum distance of the reflector from the main antenna 
             depending on the mode chosen to get the remaining distance. The remaining distance is then converted into the targetted steps the motor needs to take.
             This function uses the constant floats MinMainAntennaLength, MinReflectorAntennaDistance, LengthToStepsConversion.
             Motor steps are rounded which means the antenna has an accuracy of above or below 0.005 centimeters
             
Parameter:  mode (int)- the desired mode
            length (float)- the desired absolude length 
            conversionValue (float)- the distance the antenna holder moves in centimeters per motor step
Returns: motorSteps (int) - the number of steps the stepper motor needs to take to reqch the desired length
"""
# TODO Subtract the desired length/distance by the MinMainAntennaLength or MinReflectorAntennaDistance depending on the mode
def LengthToSteps(mode, length):
    
    MinMainAntennaLength = 5.6 # Minimum length of a main antenna (single dipole) in centimeter
    MinReflectorAntennaDistance = 4.51 # Minimum distance of the reflector antennas from the main antennas in centimeter
    MinMainAntennaHolderDistance = 7.0;
    LengthToStepsConversion = 0.01 # Main Antenna Holders moves 0.01 centimeter per motor step 
    
    # If the mode is 5 (Controlling the reflector antennas only)
    if mode == 6:
        motorSteps = int(round((length - MinReflectorAntennaDistance)/LengthToStepsConversion)) # Subtract the desired distance by the MinReflectorAntennaDistance and then convert into motor steps
    elif mode == 5:
        motorSteps = int(round((length - MinMainAntennaHolderDistance)/LengthToStepsConversion)) # Subtract the desired distance by the MinMainAntennaHolderDistance and then convert into motor steps
    # If mode is 3 (Controlling both the main antenna and reflector antenna based on the distance of the desired length of the main antenna) or 4 (Controlling the main antennas only)
    else:
        motorSteps = int(round((length - MinMainAntennaLength)/LengthToStepsConversion)) # Subtract the desired length by the MinMainAntennaLength and then convert into motor steps
    return motorSteps

"""
Function: WaitForArduino
Description: Waits for a data that will be sent from the Arduino

Parameter: none
Returns: none
"""
def WaitForArduino():
    # While there is nothing in the input buffer 
    while arduino.inWaiting == 0:
        pass # Do nothing

"""
Function: CommunicationCode
Description: Gets the reply of the Arduino and depending on the code, program continues its loop or the progam exits

Parameter: none
Returns: none
"""
def CommunicationCode():
    reply = arduino.read(2) # Read the first 2 bytes of the data sent from Arduino  
    if reply == b'00':
        print("ERROR: Inputted mode is not one of the 6 available modes") #These printed lines are for readability, they can be removed
    elif reply == b'01':
        print("Antenna Ready")
    elif reply == b'02':
        print("Antenna Homed")
    elif reply == b'03':
        print("ERROR: Misstep in belt system of main antenna")
        exit() # Exit the program to fix the mistep
    elif reply == b'04':
        print("ERROR: Misstep in gear system of reflector antennas")
        exit() # Exit the program to fix the mistep
    elif reply == b'05':
        print("ERROR: Frequency not within range")
    elif reply == b'06':
        print("ERROR: Absolute length inputted is not within capability of antennas")
    else:
        #if arduino returns a code with a value other than the above, exit the program
        exit()


"""
Function HumanInput
Description: takes user input through keyboard. This function can be removed once human input is not required

"""
def HumanInput():
    human_inputMode = 0
    while 1:
        human_inputModeStringVer = input("Enter Desired Mode: [1, 2, 3, 4, 5, 6]")
        # Check if the value inputted is an int
        try:
            human_inputMode = int(human_inputModeStringVer)
            break #break from loop once input is a valid integer
        except ValueError: #if input is not an integer
            print("Input number values only") # Indicate to user that a number value must be inputted
    
    human_lengthInput = 0
    
    #If the mode is 1, send the data to Arduino and end
    if int(human_inputMode) == 1:
        ControlAntenna(human_inputMode,0)
    else:
        while 1:
            # Check if value inputted is an input is float
            human_lengthInputStringVer = input("Enter Desired Absolute Length: ")
            try:
                human_lengthInput = float(human_lengthInputStringVer)
                break #break from loop once input is a valid float
            except ValueError: #if input is not a float
                print("Input number values only")
        
        # Send the desired mode and length to Arduino
        ControlAntenna(human_inputMode, human_lengthInput)

#Wait for the Arduino to finish homing the antennas
WaitForArduino()

#Exit program if arduino is not able to home properly (code recieved is not "02")
if arduino.read(2) != b'02':
    exit() #exits program when arduino is not homed properly

# MAIN LOOP
while 1:
    #Send Instruction to the Arduino
    HumanInput() #Remove when human input is no long necessary
                 #Use the ControlAntenna function instead
    
    #Wait for reply from arduino
    WaitForArduino()
    
    #Check Error Log 
    CommunicationCode()
    
    # Clear the output buffer to make sure no unintended data will be sent
    arduino.reset_output_buffer()