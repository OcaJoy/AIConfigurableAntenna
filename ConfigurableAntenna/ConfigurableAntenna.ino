#include <AccelStepper.h>

// Define Constants for Pins
const int antA_PHASE = 19;
const int antB_PHASE = 18;
const int refA_PHASE = 3;
const int refB_PHASE = 2;
const int ant_EnaPin = 25; // Enable pin for Main Antenna Motor
const int ref_EnaPin = 27; // Enable pin for Reflector Antenna Motor
const int ant_StepPin = 29;
const int ant_DirPin = 31;
const int ref_StepPin = 33;
const int ref_DirPin = 35;
const int ant1_LimitSwitch = 37; // Switch that activates when 1st Main Antenna is fully retracted
const int ant2_LimitSwitch = 39; // Switch that activates when 2nd Main Antenna is fully retracted
const int ref1_LimitSwitch = 41; // Switch that activates when 1st Reflector Antenna is fully retracted
const int ref2_LimitSwitch = 43; // Switch that activates when 2nd Reflector Antenna is fully retracted

// Define Min & Max Frequency
const long freqMIN = 320000000; // 320 MHz
const long freqMAX = 1600000000; // 1.6 GHz

// TODO Define Min & Max Motor Steps
const int stepsMIN = 0; 
const int stepsMAX = 0;

// Define Motor Speed
const int motorSpeed = 1200; // in Steps per Second

// Define Global Variables
unsigned long frequency;  // Frequency Variable
String dataInput; //The string of the combined data for which mode to use and the target step the moter needs to be at
char modeInput; // Holds the instruction for which mode to use
long ant_ReqStep; // Required Steps of Antenna Motor
long ref_ReqStep; // Required Steps of Reflector Antenna Motor
long ant_ENC = 0;  // Holds the Main Antenna Encoder Value      (Positive Values are CCW & Negative Valeus are CW)
long ref_ENC = 0;  // Holds the Reflector Antenna Encoder Value 

// Define variables for Caclulating Distance to step
float c = 299792458;
float dipoleLengthMIN = (c/freqMAX)/4; //Length of Dipole at Minimum Frequency (320 MHz)
float meterToStep = 0.0001;
                 
void setup() {
  // Setup Pins 
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
  Serial.begin(115200);
  Serial.setTimeout(50);
  
  // Home in Antenna at starting position
  AntennaHome(); 
}

void loop() {
  // When there is data from Raspberry Pi
  if(Serial.available())
  {
    //Instructions for Raspberry Pi
    dataInput = Serial.readString();
    modeInput = dataInput.charAt(0); // Extract the mode instruction from the string
    dataInput.remove(0,1); // Remove the mode instruction from the string

    switch(modeInput)
    {
      case '1': // Home Antennas
        // Home the antennas
        AntennaHome();
        break;

      case '2': // Move Antennas to inputted frequency
        // Convert the dataInput string into an long integer
        frequency = dataInput.toInt();

        if((frequency >= freqMIN) && (frequency <= freqMAX))
        {
          // Calculate required steps needed to reach targetted frequency
          ant_ReqStep = StepsCalc(frequency);
          ref_ReqStep = ant_ReqStep;

          // Move Antennas
          MoveMotor(ant_ReqStep, ant_StepPin, ant_DirPin, ant_ENC);
          MoveMotor(ref_ReqStep, ref_StepPin, ref_DirPin, ref_ENC);

          Serial.write("01"); // Send 01: Antenna is at targetted length
        } else {Serial.write("05");} // Send 05: Frequency is not within range
        break;

      case '3': // Move Antennas to inputted length
        // Set the steps needed to reach targetted length
        ant_ReqStep = dataInput.toInt();
        ref_ReqStep = ant_ReqStep;

        // Move Antennas
        MoveMotor(ant_ReqStep, ant_StepPin, ant_DirPin, ant_ENC);
        MoveMotor(ref_ReqStep, ref_StepPin, ref_DirPin, ref_ENC);

        Serial.write("01"); // Send 01: Antenna is at targetted length
        break;

      case '4': // Only move Main Antenna to inputted length
        // Set the steps needed to reach targetted length
        ant_ReqStep = dataInput.toInt();

        //Move Main Antenna
        MoveMotor(ant_ReqStep, ant_StepPin, ant_DirPin, ant_ENC);

        Serial.write("01"); // Send 01: Antenna is at targetted length
        break;

      case '5':
        //Set the steps needed to reach targetted length
        ref_ReqStep = dataInput.toInt();

        //Move Main Antenna
        MoveMotor(ant_ReqStep, ant_StepPin, ant_DirPin, ant_ENC);

        Serial.write("01"); // Send 01: Antenna is at targetted length
        break;
    }
  }
}

/***********************************************************
 * Method enableMotors
 * param bool state - boolean variable that determines if both motors are enabled or disabled
 * returns void
 * 
 * Enables or disables both motors through the motor driver depending on what is inputted in the parameter "state"
 *  0 - Enables the motor
 *  1 - Disables the motor
************************************************************/
void EnableMotors(bool state)
{
  digitalWrite(ant_EnaPin, state);
  digitalWrite(ref_EnaPin, state);
}

/***********************************************************
 * method antHome
 * returns void
************************************************************/
void AntennaHome()
{ 
  // Create AccelStepper Objects
  AccelStepper AntStepper(1, ant_StepPin, ant_DirPin);
  AccelStepper RefStepper(1, ref_StepPin, ref_DirPin);

  // Set speeds for both motors
  AntStepper.setMaxSpeed(800);
  AntStepper.setSpeed(800);
  RefStepper.setMaxSpeed(800);
  RefStepper.setSpeed(800);
  
  // Enable both motors
  EnableMotors(0);
  
  // Home the Main Antenna
  while(!(digitalRead(ant1_LimitSwitch) || digitalRead(ant2_LimitSwitch)))
  {
    AntStepper.runSpeed();
  }

  // Check that both limit switches of Main Antenna have been activated
  if(!(digitalRead(ant1_LimitSwitch) && digitalRead(ant2_LimitSwitch)))
  {
    Serial.write("03"); // Send 03: There is a mistep in the belt system of main antennas
    return; 
  }

  // Home the Reflector Antenna
  while(!(digitalRead(ref1_LimitSwitch) || (digitalRead(ref2_LimitSwitch))))
  {
    RefStepper.runSpeed();
  }

  // Check that both limit switches of Reflector Antenna have been activated
  if(!(digitalRead(ref1_LimitSwitch) && (digitalRead(ref2_LimitSwitch))))
  {
    Serial.write("04"); // Send 04: There is a mistep in gear system of reflector antennas
  }
  
  // Reset the Encoder Values
  ant_ENC = 0;
  ref_ENC = 0;

  // Disable both motors
  EnableMotors(1);

  Serial.write("02"); // Send 02: All antennas homed properly
}

/***********************************************************
 * Method: antEncoder
 * returns void
 * 
 * This method is called when the main antenna encoder interrupt is triggered
 * Increases the main antenna encoder value by 1 when encoder is stepped once counter-clockwise and decreases the value by 1 when encoder is stepped once clockwise.
************************************************************/
void antEncoder()
{
  char i;
  i = digitalRead(antB_PHASE);
  if (i==1) // Turning CCW
    ant_ENC += 1;
  else // Turning CW
    ant_ENC -= 1;
}

/***********************************************************
 * Method: refEncoder
 * returns void
 * 
 * This method is called when the reflector antenna encoder interrupt is triggered
 * Increases the main antenna encoder value by 1 when encoder is stepped once counter-clockwise and decreases the value by 1 when encoder is stepped once clockwise.
************************************************************/
void refEncoder()
{
  char i;
  i = digitalRead(refB_PHASE);
  if (i==1) // Turning CCW
    ref_ENC += 1;
  else // Turning CW
    ref_ENC -= 1;
}

/***********************************************************
 * Method: StepsCalc
 * param unsigned long freq - the frequency the antenna has to tune to
 * returns steps - the number of steps the motor has to move to attain the required length
 * 
 * Calculates the dipole length based on the frequency inputted.
 * Calculates the dipole length it needs to shorten/elongate into by subtracting the dipole length with the length of the dipole at minimum frequency.
 * Converts the dipole length it needs to shorten/elongate into steps and returns that value
************************************************************/
long StepsCalc(unsigned long freq)
{
  float dipoleLength, dipoleLengthToGo;
  long steps;  

  // Find Antenna Length Required for input frequency
  dipoleLength = (c/freq)/4;

  // Subtract Antenna Length by Minimum Length
  dipoleLengthToGo = dipoleLength - dipoleLengthMIN; 
  
  // Convert resulting length into # of steps 
  steps = dipoleLengthToGo/meterToStep;
  
  // Return Steps
  return steps;
}

/***********************************************************
 * Method: MotorMove
 * param long ReqStep - the number of steps the motor is required to move
 *       long StepPin - the pin that sends data on how much needs to be stepped on the motor driver
 *       long DirPin  - the pin that controls whether the motor runs clockwise or counter-clockwise on the motor driver
 *       long Encoder - encoder variable of the current motor that will be moved      
 * returns void
 * 
 * Initally creates an instance of an AccelStepper Class that uses the Step and Direction Pin of the motor driver specified.
 * Calculates the amount of steps needed to reach the required steps based on the current step in the encoder variable and move the motor. 
 * Checks if the encoder value is equal to the required steps. If not, calculate missing required steps and move the motor until both values are equal.
************************************************************/
void MoveMotor(long ReqStep, long StepPin, int DirPin, long Encoder)
{
  //Create Accel Stepper Object
  AccelStepper stepper(1, StepPin, DirPin);
  stepper.setMaxSpeed(motorSpeed);
  
  // Move Motor to required step
  stepper.move(ReqStep - Encoder);
  stepper.setSpeed(motorSpeed);

  // Enable Motors
  EnableMotors(0);

  // Keep running motor until distance is reached
  while(stepper.distanceToGo() != 0)
  {
    stepper.runSpeedToPosition(); // Command to step motor at constant speed specified
  }

  // Match Encoder reading to required step
  while(Encoder != ReqStep)
  {
    stepper.move(ReqStep - Encoder);
    stepper.setSpeed(800);  // Set at lower speed for precise movements

    while(stepper.distanceToGo() != 0)
    {
      stepper.runSpeedToPosition();
    }
  }

  // Disable Motors:
  EnableMotors(1);
}
