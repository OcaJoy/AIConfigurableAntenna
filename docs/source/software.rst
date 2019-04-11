Software
========

1. Getting Started
------------------
| To run this program, the following software are needed:

- `Python <https://www.python.org/downloads/>`_

- `Arduino <https://www.arduino.cc/en/main/software>`_: required to edit and upload the program to the Arduino Mega


2. Libraries
------------
- `PySerial <https://pypi.org/project/pyserial/>`_ [Version 3.4]

  | This library is used to establish a serial connection between python and arduino to be able to read and write data between the two systems.
  
- `AccelStepper <https://www.airspayce.com/mikem/arduino/AccelStepper/>`_ [Version 1.59] 

  | This library is used to send signals to the Pololu A4988 Stepper Motor Driver to be able to drive the Stepper Motors.


3. Raspberry Pi Code (RPiArduinoCom.py)
---------------------------------------
3.1. Main Program Description
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
3.1.1. Setup
^^^^^^^^^^^^
| The program begins by creating a `Serial` object named `arduino` at port `"/dev/ttyACM0"` with a baudrate of 115200. After opening up the specified serial port, the program waits for the Arduino to finish homing the antennas by waiting for a reply. If the reply received from Arduino not code: 02, the program exits.   
| *See the* `Communication Codes`_ *section, for more information on what each code mean*

3.1.2. Main Loop
^^^^^^^^^^^^^^^^
| The loop begins by either calling the ``ControlAntenna (modeInput, lengthInput)`` or  the ``HumanInput()`` function to send instructions to the Arduino. When using either of the functions, the user is required to input the desired mode and the desired absolute length of the antenna.  
| *For information on the modes available, see the* :ref:`modesofoperation`.
|
| Once the desired input has been sent, the program waits for a reply from Arduino. The Arduino will then send back data to the Raspberry Pi after it has completed its instruction. The program then reads the reply and determines whether the Arduino successfully performed the instruction or if an error occurred. The last task in the loop is to clear the output buffer to make sure no unintended data will be sent to the Arduino. The program then loops back into asking for another instruction and the cycle continues.   

3.2. Imports
~~~~~~~~~~~~
| Program imports Pyserial to create an object that can serially communicate with the Arduino via USB Cable.  
| ``from serial import Serial``

3.3. Global Variables
~~~~~~~~~~~~~~~~~~~~~
- ``port = "/dev/ttyACM0"``: Contains the serial port to be communicated with by the `Serial` object

- ``baudrate = 115200`` (int): Contains the value at which baudrate both devices will serially communicate in

- ``arduino = Serial(port, baudrate)``: The variable containing the object that opens a serial port at the specified port and baudrate

- ``LengthToStepsConversion = 0.01`` (float): Contains the conversion factor of 0.01 centimeters per 1 step of the motor. The desired absolute length of the antenna inputted is divided by this value to get the number of steps needed by the motor to achieve that length.  

- ``MinMainAntennaLength = 5.6`` (float): Contains the length of the main antenna at its retracted end position in centimeters. This is subtracted from the desired absolute antenna length to find the distance left needed for the main antenna motor to travel.

- ``MinReflectorAntennaDistance = 4.51`` (float): Contains the distance of the reflector antenna from the main antenna at its retracted end position in centimeters. This is subtracted from the desired absolute reflector distance to find the distance left needed for the reflector antenna motor to travel. 

3.4. Function Definitions
~~~~~~~~~~~~~~~~~~~~~~~~~
3.4.1 ``ControlAntenna(modeInput, lengthInput)``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- **Description:** 

  | Takes the desired mode and desired absolute antenna length that is inputted in the parameters and sends them to the Arduino; it converts inputs into a string, combines them, and then sends the combined string to the Arduino. 

- **Parameters:** 

  - ``modeInput`` (int): Input an integer from 1-5 to choose a corresponding mode from :ref:`modesofoperation`
  - ``lengthInput`` (int/float): Input the desired length integer or float depending on the mode listed below

    - ``modeInput = 1``: must input a 0
    - ``modeInput = 2``: must input an frequency within 320 MHz to 1.6 GHz (integers only)
    - ``modeInput = 3``: must input the desired absolute length of the main antenna within 5.6 - 23 centimeters
    - ``modeInput = 4``: must input the desired absolute length of the main antenna within 5.6 - 23 centimeters
    - ``modeInput = 5``: must input the desired absolute distance of the reflector antennas within 4.51 - 24 centimeters
- **Returns:** None

3.4.2 ``HumanInput()``
^^^^^^^^^^^^^^^^^^^^^^
- **Description:** 

  | Takes a human input via keyboard. This function will check if the values inputted for the desired mode and absolute antenna length are in integers or floats and will keep asking the user to enter again until a valid type is inputted. This function then calls for ``ControlAntenna(modeInput, lengthInput)`` inputting the desired mode to ``modeInput`` and the desired absolute antenna length or frequency to the ``lengthInput``.  
  | This function can be removed when human input is not required.

- **Parameters:** None
- **Returns:** None

3.4.3 ``LengthToSteps(mode, length, conversionValue)``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- **Description:**

  | This function subtracts the desired absolute length of the antenna with the minimum length of the antenna (`MinMainAntennaLength`) or the minimum distance of the reflector from the main antenna (`MinReflectorAntennaDistance`) depending on the mode chosen to get the remaining distance. The remaining distance is then converted into the targeted steps the motor needs to take by dividing it with the constant ``LengthToStepsConversion``.  

- **Parameters:**

  - ``mode`` (int): Input the desired mode
  - ``lengthInput`` (int / float): Input the desired absolute length of the antenna
  - ``conversionValue`` (float):  Uses the set value of ``LengthToStepsConversion`` to convert the remaining length needed to reach the desired absolute length of the antenna into motor steps
- **Returns:** ``motorSteps`` (int) 

3.4.4. ``WaitForArduino()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^
- **Description:** 

  | This function will check the serial input buffer and will loop doing nothing as long as it is empty. The loop only stops when a reply is received from the Arduino. 

- **Parameters:** None
- **Returns:** None

3.4.5. ``CommunicationCode()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- **Description:** 

  | Gets the reply of the Arduino and depending on the code, the program either continues its loop or the program exits.  
  | *See the* `5. Communication Codes`_ *section to see what the the program will perfrom based on the reply.*

- **Parameters:** None
- **Returns:** None


4. Arduino Code (ConfigurableAntenna.ino)
-----------------------------------------
4.1. Main Program Description
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
4.1.1. Setup
^^^^^^^^^^^^
| Before the main loop of the program begins, the Arduino sets up by determining which of the pins used are inputs and outputs. It then attaches an interrupt to the pins connected to the encoders of the main antennas and reflector antennas. This means that an interrupt will occur when the encoders of the main antennas or the reflector antennas detect a rotation, which will then increment the value that stores how much the motor has rotated in steps. Next, it opens up a serial connection with a baudrate the same as the Raspberry Pi to communicate with it. Lastly, the Arduino homes all the antennas and sends a message to the Raspberry Pi when it is done.

4.1.2. Main Loop
^^^^^^^^^^^^^^^^
| The main loop always checks the serial input buffer if there is a message from the Raspberry Pi. If there is a message, the instruction from the Raspberry Pi is extracted and broken down into two different variables; One variable contains the mode the Arduino will perform, and the other variable contains the frequency desired or the desired step the motor needs to rotate to. The program then performs the mode instructed by the Raspberry Pi.
|
| For modes 2-5, the Arduino will always check if the inputted frequency or length is within the capability of the Configurable Antenna. If it is within its capability, the Arduino does its task and sends a code of "01" back to the Raspberry Pi that it has completed its task and is ready for another one. If it is not within its capability, it sends an error code to the Raspberry Pi.  
| *See the* `5. Communication Codes`_ *section for more information*

4.2. Include
~~~~~~~~~~~~
| The program includes the AccelStepper Library that gives instructions to the motor driver to move the motor.  
| ``#include <AccelStepper.h>``

4.3. Pins
~~~~~~~~~
The value contained is the Arduino Pin Number it is connected to.
 
- ``const int antA_PHASE = 19;``: Pin of the main antenna encoder that triggers the interrupt 
- ``const int antB_PHASE = 18;``: Pin of the main antenna encoder that determines the direction the encoder is revolving in
- ``const int refA_PHASE = 3;``: Pin of the reflector antenna encoder that triggers the interrupt
- ``const int refB_PHASE = 2;``: Pin of the reflector encoder that determines the direction the encoder is revolving in
- ``const int ant_EnaPin = 25;``: Enable pin of the motor driver of the main antenna motor
- ``const int ref_EnaPin = 27;``: Enable pin of the motor driver of the reflector antenna motor
- ``const int ant_StepPin = 29;``: Pin of the motor driver that controls the rotation of the main antenna motor
- ``const int ant_DirPin = 31;``: Pin of the motor driver that controls the rotation direction of the main antenna motor
- ``const int ref_StepPin = 33;``: Pin of the motor driver that controls the rotation of the reflector antenna motor
- ``const int ref_DirPin = 35;``: Pin of the motor driver that controls the rotation direction of the main antenna motor
- ``const int ant1_LimitSwitch = 37;``: Switch that activates when 1st Main Antenna is fully retracted
- ``const int ant2_LimitSwitch = 39;``: Switch that activates when 2nd Main Antenna is fully retracted
- ``const int ref1_LimitSwitch = 41;``: Switch that activates when 1st Reflector Antenna is fully retracted
- ``const int ref2_LimitSwitch = 43;``: Switch that activates when 2nd Reflector Antenna is fully retracted

4.4. Global Variables
~~~~~~~~~~~~~~~~~~~~~
- ``const long freqMIN = 320000000;``: The minimum frequency that the antenna can extend to (320 MHz)

- ``const long freqMAX = 1600000000;``: The maximum frequency that the antenna can shorten to (1.6 GHz)

- ``const int motorSpeed = 1200;``: The speed of the motor in steps per second (with a 400 stepper motor the speed would be 3 revolutions per second)

- ``unsigned long frequency;``: Stores the frequency inputted by the user

- ``String dataInput;``: Contains the combined data taken from the Raspberry Pi for which mode to use and the desired step the motor needs to be

- ``char modeInput;``: Contains the instruction on which mode the Arduino must move the motor in

- ``long ant_ReqStep;``: Contains the required motor steps the main antenna motor needs to move to reach the desired length 

- ``long ref_ReqStep;``: Contains the required motor steps the reflector antenna motor needs to move to reach the desired length

- ``long ant_ENC = 0;``: Contains the main antenna encoder value (Positive values are how many steps the motor has rotated counter-clockwise)

- ``long ref_ENC = 0;``: Contains the reflector antenna encoder value (Positive values are how many steps the motor has rotated counter-clockwise)

4.5. Function Definitions
~~~~~~~~~~~~~~~~~~~~~~~~~
4.5.1. ``EnableMotors(bool state)``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- **Description:** 

  | Enables or disables both the main antenna and reflector antenna motors depending on the inputted `state`. 

- **Parameters:**

  - ``bool state``: an input that determines if both motors are enabled or disabled

    - ``state = 0`` - Enables both motors
    - ``state = 1`` - Disables both motors
- **Returns:** void

4.5.2. ``AntennaHome()``
^^^^^^^^^^^^^^^^^^^^^^^^
- **Description:**
 
  | Moves the main antenna and reflector antennas back to their retracted end position at a lower speed. This function makes use of the inputs of the 4 microswitches: ``ant1_LimitSwitch``, ``ant2_LimitSwitch``, ``ref1_LimitSwitch``, and ``ref2_LimitSwitch`` to check if all the antennas are homed properly.  
  | If either one of the main antenna microswitches is not activated after homing, then there is a mistep in the belt system of the main antenna.   
  | If either one of the reflector antenna microswitches is not activated after homing, then there is a mistep in the gear system of the reflector antenna.  
  | If antennas are homed properly, the function sends a code "02" to the Raspberry Pi.

- **Parameters:** None
- **Returns:** void

4.5.3. ``antEncoder()``
^^^^^^^^^^^^^^^^^^^^^^^
- **Description:** 

  | This function is called when the main antenna encoder triggers an interrupt when it detects a step in the motor. It increases the `long ant_ENC` encoder value by 1 when the motor is stepped once counter-clockwise and decreases the value by 1 when the motor is stepped once clockwise.

- **Parameters:** None
- **Returns:** None

4.5.4. ``refEncoder()``
^^^^^^^^^^^^^^^^^^^^^^^
- **Description:** 

  | This function is called when the reflector antenna encoder triggers an interrupt when it detects a step in the motor. It increases the `long ref_ENC` encoder value by 1 when the motor is stepped once counter-clockwise and decreases the value by 1 when the motor is stepped once clockwise.

- **Parameters:** None
- **Returns:** void

4.5.5. ``StepsCalc(unsigned long freq)``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- **Description:** 

  | Calculates the length the main antennas need to shorten/elongate into based on the frequency inputted by subtracting the main antenna length desired with the main antenna length at minimum frequency.

- **Parameters:**  

  - ``unsigned long freq``: the frequency the antenna has to shorten/elongate into to tune to
- **Returns:** void

4.5.6. ``MoveMotor(long ReqStep, int StepPin, int DirPin, long Encoder)``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- **Description:** 

  | Moves the specified motor based on the direction and step pin inputted to the desired steps based on the required steps inputted. The encoder variable is used to check the current position of the antenna to see if it needs to shorten or extend to the required step.   
  | This function uses commands from the `AccelStepper Library <https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html>`_ to send instructions to the motor driver to move the motor.

- **Parameters:** 
 
  - ``long ReqStep``: the number of steps the motor is required to move
  - ``int StepPin``: the pin of the motor driver that controls the motor 
  - ``int DirPin``: the pin of the motor driver that controls the direction of the motor
  - ``long Encoder``: encoder variable of the desired motor to be moved 
- **Returns:** void


5. Communication Codes
----------------------
5.1. Codes
~~~~~~~~~~
The codes that will be sent by the Arduino depending on the success or failure of its performance.

- **00**: ERROR: Mode inputted not valid  

  | This code is sent when the mode inputted is not one of the 5 modes available

- **01**: Antenna ready  

  | This code is sent when the Arduino successfully moved the main antennas and the reflector antennas to the desired length. 

- **02**: Antenna homed  

  | This code is sent when the Arduino successfully homed all the antennas

- **03**: ERROR: Misstep in the belt system of the main antennas  

  | This code is sent when 1 of the 2 switches that checks if the main antennas are homed is not activated to notify that there is a misstep in the belt system of the main antenna. The Raspberry Pi program exits so that the belt system can be readjusted. 

- **04**: ERROR: Misstep in the gear system of the reflector antennas.

  | This code is sent when 1 of the 2 switches that check if the reflector antennas are homed is not activated to notify that there is a misstep in the gear system of the reflector antenna. The Raspberry Pi program exits so that the gear system can be readjusted.

- **05**: ERROR: Frequency inputted is not within the capability of antennas.  
  | This code is sent to notify that the inputted frequency is not within 320 MHz - 1.6 GHz

- **06**: ERROR: Desired antenna length inputted is not within the capability of antennas.    

  | This code is sent to notify that the inputted antenna length is not within its range 

5.2. How the Arduino sends the codes to the Raspberry Pi
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| The Arduino uses the `Serial.write()` command to send the code in bytes. The Arduino uses a total of 2 bytes for sending the communication code to the Raspberry Pi.

5.3. How the Raspberry Pi reads the received codes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| The Raspberry Pi uses the `.read(2)` command of Pyserial to read the 2 incoming bytes of the communication code. 
| If the Raspberry Pi receives a code that is not 00-06, the Raspberry Pi program exits because there is an unforeseen communication error.  




