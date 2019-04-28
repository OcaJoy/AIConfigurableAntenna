#include <AccelStepper.h>

// Define Constants for Pins
const int antA_PHASE = 3;
const int antB_PHASE = 30;
const int refA_PHASE = 2;
const int refB_PHASE = 31;
const int ant_EnaPin = 24; // Enable pin for Main Antenna Motor
const int ref_EnaPin = 25; // Enable pin for Reflector Antenna Motor
const int ant_StepPin = 26;
const int ant_DirPin = 28;
const int ref_StepPin = 27;
const int ref_DirPin = 29;
const int ant1_LimitSwitch = 33; // Switch that activates when 1st Main Antenna is fully retracted
const int ant2_LimitSwitch = 35; // Switch that activates when 2nd Main Antenna is fully retracted
const int ref1_LimitSwitch = 37; // Switch that activates when 1st Reflector Antenna is fully retracted
const int ref2_LimitSwitch = 39; // Switch that activates when 2nd Reflector Antenna is fully retracted

// Define Min & Max Frequency
const long freqMIN = 326000000; // 326 MHz
const long freqMAX = 1314000000; // 1.314 GHz

// Define Motor Speed
const int motorSpeed = 1000; // in Steps per Second

// Define Global Variables
unsigned long frequency;  // Frequency Variable
String dataInput; //The string of the combined data for which mode to use and the target step the moter needs to be at
char modeInput; // Holds the instruction for which mode to use
long ant_ReqStep; // Required Steps of Antenna Motor
long ref_ReqStep; // Required Steps of Reflector Antenna Motor
long ant_ENC = 0;  // Holds the Main Antenna Encoder Value      (Positive Values are CCW & Negative Valeus are CW)
long ref_ENC = 0;  // Holds the Reflector Antenna Encoder Value 

// Define variables for Caclulating Distance to step
float c = 299792458; // Speed of light in meters
float AntennaLengthMIN = 5.7; // Minimum length of the main antenna in centimeters
float conversionValue = 0.01; // Conversion value used to convert the length needed in centimeters to steps needed

/**********************************************************************************************************************************************************************************               
 * The program sets up by:
 *  - Indicating which pins are inputs and outputs
 *  - Attaching the interrupt function to the encoders
 *  - Opening up a Serial Connection with the Raspberry Pi                
 *  - Homing the antennas on startup 
**********************************************************************************************************************************************************************************/
void setup() {
  // Indicate which pins are inputs and outputs
  pinMode(antA_PHASE, INPUT);
  pinMode(antB_PHASE, INPUT);
  pinMode(refA_PHASE, INPUT);
  pinMode(refB_PHASE, INPUT);
  pinMode(ant1_LimitSwitch, INPUT);
  pinMode(ant2_LimitSwitch, INPUT);
  pinMode(ref1_LimitSwitch, INPUT);
  pinMode(ref2_LimitSwitch, INPUT);
  pinMode(ant_StepPin, OUTPUT);
  pinMode(ant_DirPin, OUTPUT);
  pinMode(ref_StepPin, OUTPUT);
  pinMode(ref_DirPin, OUTPUT);
  pinMode(ant_EnaPin, OUTPUT);
  pinMode(ref_EnaPin, OUTPUT);
  
  // Attach Interrupt to both Rotary Encoders
  attachInterrupt(digitalPinToInterrupt(antA_PHASE), antEncoder, RISING); //Interrupt triggers on Rising Edge
  attachInterrupt(digitalPinToInterrupt(refA_PHASE), refEncoder, RISING); //Interrupt triggers on Rising Edge

  // Set up Serial Connection
  Serial.begin(115200); //Set up the baudrate (has to be the same as what is set on the Raspberry Pi)
  Serial.setTimeout(50);

  // Home the Antennas
  AntennaHome();
}

/*********************************************************************************************************************************************************************************
 * This main function always checks the serial input buffer if there is a message from the Raspberry Pi. 
 * If there is a message, the instructions from the Raspberry Pi is extracted and broken down to read the desired mode and length.
 * The Arduino moves the motor based on the desired inputs.
**********************************************************************************************************************************************************************************/
void loop() {
  // When there is data from Raspberry Pi
  if(Serial.available())
  {
    //Instructions from Raspberry Pi
    dataInput = Serial.readString(); //Get the full instruction from Raspberry Pi and place it in a string
    modeInput = dataInput.charAt(0); // Extract the mode instruction from the first character of the string
    dataInput.remove(0,1); // Remove the first character or the mode instruction from the string

    switch(modeInput)
    {
      // Home Antennas
      case '1': 
        // Home the antennas
        AntennaHome();
        break;

      // Move Antennas to inputted frequency
      case '2': 
        // Convert the dataInput string into an long integer
        frequency = dataInput.toInt();

        if((frequency >= freqMIN) && (frequency <= freqMAX))
        {
          // Calculate required steps needed to reach targetted frequency
          ant_ReqStep = StepsCalc(frequency);

          // Check ant_ReqSteps is between 0-90 steps (5.7 - 6.6 centimeters)
          if((ant_ReqStep >= 0) && (ant_ReqStep <= 90))
          {
            ref_ReqStep = 0; // Reflector Antenna Motor will stay at its minimum distance since it cannot go any lower to match the main antenna's distance
          }
          else if(ant_ReqStep >= 1160) // If ant_ReqSteps is beyond 1210 steps (18.7 centimeters) 
          {
            ref_ReqStep = 1160; // Reflector Antenna Motor will stay at its maximum distance since it cannot go any further to match the main antenna's distance
          }
          else // If the ant_ReqSteps is between 90 - 1210 steps (6.6 - 18.7 centimeters)
          {
            ref_ReqStep = ant_ReqStep - 90; // Decrease the required steps of the Reflector Antenna Motor by 90 steps due to the 0.9 cm difference in minimum distances of the Reflector Antenna and Main Antenna
          }
          
          // Move Antennas
          MoveMotor(ant_ReqStep, 1, ant_StepPin, ant_DirPin);
          MoveMotor(ref_ReqStep, 2, ref_StepPin, ref_DirPin);

          Serial.write("01"); // Send 01: Antenna is at targetted length
        } else {Serial.write("05");} // Send 05: Frequency is not within range
        break;

      // Move Antennas to inputted length
      case '3': 
        // Set the steps needed to reach targetted length
        ant_ReqStep = dataInput.toInt();
        
        // Check ant_ReqSteps is between 0-90 steps (5.7 - 6.6 centimeters)
        if((ant_ReqStep >= 0) && (ant_ReqStep <= 90))
        {
          ref_ReqStep = 0; // Reflector Antenna Motor will stay at its minimum distance since it cannot go any lower to match the main antenna's distance
        }
        else if(ant_ReqStep >= 1160) // If ant_ReqSteps is beyond 1160 steps (18.2 centimeters) 
        {
          ref_ReqStep = 1160; // Reflector Antenna Motor will stay at its maximum distance since it cannot go any further to match the main antenna's distance
        }
        else // If the ant_ReqSteps is between 90 - 1160 steps (6.6 - 18.2 centimeters)
        {
          ref_ReqStep = ant_ReqStep - 90; // Decrease the required steps of the Reflector Antenna Motor by 90 steps due to the 0.9 cm difference in minimum distances of the Reflector Antenna and Main Antenna
        }
        
        // Check if the length input is within the capability of the antenna
        if((ant_ReqStep >= 0) && (ant_ReqStep <= 1730))
        {
          // Move Antennas
          MoveMotor(ant_ReqStep, 1, ant_StepPin, ant_DirPin);
          MoveMotor(ref_ReqStep, 2, ref_StepPin, ref_DirPin);

          Serial.write("01"); // Send 01: Antenna is at targetted length
        } else {Serial.write("06");} // Send 06: Length inputted is outside antenna capability
        break;
        
      // Case 4 & 5 Uses the same instruction as the Raspberry Pi will handle the difference in distancing 
      case '4': // Only move Main Antenna to inputted length
      case '5': // Only move Main Antenna Holders to inputted length from the center
        // Set the steps needed to reach targetted length
        ant_ReqStep = dataInput.toInt();

        // Check if the length input is within the capability of the antenna
        if((ant_ReqStep >= 0) && (ant_ReqStep <= 1730))
        {
          //Move Main Antenna
          MoveMotor(ant_ReqStep, 1, ant_StepPin, ant_DirPin);

          Serial.write("01"); // Send 01: Antenna is at targetted length
        } else {Serial.write("06");} // Send 06: Length inputted is outside antenna capability
        break;

      // Only move Reflector Antenna to inputted length
      case '6':
        //Set the steps needed to reach targetted length
        ref_ReqStep = dataInput.toInt();

        // Check if the length input is within the capability of the antenna
        if((ref_ReqStep >= 0) && (ref_ReqStep <= 1210))
        {
          //Move Main Antenna
          MoveMotor(ref_ReqStep, 2, ref_StepPin, ref_DirPin);

          Serial.write("01"); // Send 01: Antenna is at targetted length
        } else {Serial.write("06");} // Send 06: Length inputted is outside antenna capability
        break;

      default:
        Serial.write("00"); // Send 00: Not a valid mode is given
    }
  }
}

/*********************************************************************************************************************************************************************************
 * Method enableMotors
 *  Enables or disables both motors through the motor driver depending on what is inputted in the parameter "state"
 *  0 - Enables the motor
 *  1 - Disables the motor
 *  
 * Parameter:  bool state - boolean variable that determines if both motors are enabled or disabled
 * Returns: void
**********************************************************************************************************************************************************************************/
void EnableMotors(bool state)
{
  digitalWrite(ant_EnaPin, state);
  digitalWrite(ref_EnaPin, state);
}

/*********************************************************************************************************************************************************************************
 * method antHome
 * Moves the main antenna and reflector antennas back to their retracted end position at a lower speed. 
 * This function makes use of 4 microswitches (2 for the main antennas and the other 2 for the reflector antennas).
 * The homing of the main antenna completes when either of the 2 main antenna microswitch is activated
 * The homing of the reflector antenna completes when either of the 2 reflector antenna microswitch is activated
 * The same logic applies to the microswitches of the reflector antennas.
 * 
 * Parameter:  none
 * Returns:  void
**********************************************************************************************************************************************************************************/
void AntennaHome()
{ 
  // Create AccelStepper Objects
  AccelStepper AntStepper(1, ant_StepPin, ant_DirPin);
  AccelStepper RefStepper(1, ref_StepPin, ref_DirPin);

  // Set max speeds for both motors
  AntStepper.setMaxSpeed(800);
  RefStepper.setMaxSpeed(800);
  
  // Enable both motors
  EnableMotors(0);
  
  // Home the Main Antenna
  while(digitalRead(ant1_LimitSwitch) == LOW  && digitalRead(ant2_LimitSwitch) == LOW) // Motor will continue to retract as long as either switch has not been activated
  {
    AntStepper.move(-90); // Set the motor to move in small steps in the retracting direction
    AntStepper.setSpeed(600); // Set Speed

    // Move the motor
    while(AntStepper.distanceToGo() != 0)
    {
      AntStepper.runSpeedToPosition();
    } 
  }

  // Home the Reflector Antenna
  while(digitalRead(ref1_LimitSwitch) == LOW && digitalRead(ref2_LimitSwitch) == LOW) // Motor will continue to retract as long as either switch has not been activated
  {
    RefStepper.move(-150); // Set the motor to move slowly in the retracting direction
    RefStepper.setSpeed(200); //Set Speed

    // Move the motor
    while(RefStepper.distanceToGo() != 0)
    {
      RefStepper.runSpeedToPosition();
    } 
  }
  
  // Disable both motors
  EnableMotors(1);

  // Set a small delay to make sure motor is at complete stop
  delay(200);
  
  // Reset the Encoder Values
  ant_ENC = 0;
  ref_ENC = 0;
  
  Serial.write("02"); // Send 02: All antennas homed properly
}

/*********************************************************************************************************************************************************************************
 * Method: antEncoder
 * This method is called when the main antenna encoder interrupt is triggered
 * Increases the main antenna encoder value by 1 when encoder is stepped once counter-clockwise and decreases the value by 1 when encoder is stepped once clockwise.
 * 
 * Parameter:  none
 * Returns: void
**********************************************************************************************************************************************************************************/
void antEncoder()
{
  char i;
  i = digitalRead(antB_PHASE);
  if (i==1) // Turning CW
    ant_ENC -= 1;
  else // Turning CCW
    ant_ENC += 1;
}

/*********************************************************************************************************************************************************************************
 * Method: refEncoder
 * This method is called when the reflector antenna encoder interrupt is triggered
 * Increases the main antenna encoder value by 1 when encoder is stepped once counter-clockwise and decreases the value by 1 when encoder is stepped once clockwise.
 * 
 * Parameter:  none
 * Returns: void
**********************************************************************************************************************************************************************************/
void refEncoder()
{
  char i;
  i = digitalRead(refB_PHASE);
  if (i==1) // Turning CW
    ref_ENC -= 1;
  else // Turning CCW
    ref_ENC += 1;
}

/*********************************************************************************************************************************************************************************
 * Method: StepsCalc
 * Calculates the length the main antennas need to shorten or elongate based on the frequency inputted by subtracting the dipole length with the length of the main antenna at minimum frequency
 * 
 * Parameter: unsigned long freq - the frequency the antenna has to tune to
 * Returns: steps - the number of steps the motor has to move to attain the required length
**********************************************************************************************************************************************************************************/
long StepsCalc(unsigned long freq)
{
  float dipoleLength, dipoleLengthToGo;
  long steps;  

  // Calculates the length of a single dipole based on the frequency inputted
  dipoleLength = ((c/freq)/4)*100;
            // Multiplied by 100 to convert meter reading into centimeter 

  // Subtract Antenna Length by Minimum Length
  dipoleLengthToGo = dipoleLength - AntennaLengthMIN; 
  
  // Convert resulting length into # of steps (results are rounded giving an accurasy of above or below 0.005 centimeters)
  steps = round(dipoleLengthToGo/conversionValue);
  
  // Return Steps
  return steps;
}

/*********************************************************************************************************************************************************************************
 * Method: MotorMove
 * Moves the specified motor based on the direction and step pin inputted to the desired steps inputted
 * 
 * Parameter: long ReqStep - the number of steps the motor is required to move
 *            int Motor - determines which encoder variable to use with the motor (should match which motor will be used)
 *                      - Motor = 1: use the Main Antenna Encoder   
 *                      - Motor = 2: use the Reflector Antenna Encoder
 *            int StepPin - the pin of the motor driver that controls the motor
 *            int DirPin - the pin of the motor driver that controls the direction
 * Returns: void
**********************************************************************************************************************************************************************************/
void MoveMotor(long ReqStep, int Motor, int StepPin, int DirPin)
{
  // Create an AccelStepper object that use the inputted Step and Direction Pin of the motor driver specified
  AccelStepper stepper(1, StepPin, DirPin);

  // Set the max speed of the motor
  stepper.setMaxSpeed(motorSpeed);

  // Check if which motor will be used
  if(Motor == 1) // If the Main Antenna Motor is used 
  {
    // Calculate the amount of steps needed to reach the required steps based on the current steps in the Main Antenna Encoder
    stepper.move(ReqStep - ant_ENC);
  }
  else // If the Reflector Antenna Motor is used
  {
    // Calculate the amount of steps needed to reach the required steps based on the current steps in the Reflector Antenna Encoder
    stepper.move(ReqStep - ref_ENC);
  }

  // Set the speed of the motor
  stepper.setSpeed(motorSpeed);

  // Enable Motors
  EnableMotors(0);

  // Keep running motor until the desired step is reached
  while(stepper.distanceToGo() != 0)
  {
    stepper.runSpeedToPosition(); // Command to step motor at constant speed specified
  }

  // Check which Motor is used to determine which Encoder should be used for the closed loop correction of the motor chosen
  if(Motor == 1)
  {
    // Match Main Antenna Encoder reading to required steps
    while(ant_ENC != ReqStep)
    {
      stepper.move(ReqStep - ant_ENC);
      stepper.setSpeed(800);  // Set at lower speed for precise movements

      while(stepper.distanceToGo() != 0)
      {
        stepper.runSpeedToPosition();
      }
    }
  }
  else
  {
    // Match Reflector Encoder reading to required step
      while(ref_ENC != ReqStep)
    {
      stepper.move(ReqStep - ref_ENC);
      stepper.setSpeed(800);  // Set at lower speed for precise movements

      while(stepper.distanceToGo() != 0)
      {
      stepper.runSpeedToPosition();
      }
    }
  }
  
  // Disable Motors:
  EnableMotors(1);
}
