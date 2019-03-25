#include <AccelStepper.h>

// Define Constants for Pins
const int AA_PHASE = 19;
const int AB_PHASE = 18;
const int BA_PHASE = 3;
const int BB_PHASE = 2;
const int ant_StepPin = 23;
const int ant_DirPin = 25;
const int ref_StepPin = 27;
const int ref_DirPin = 29;
const int ant1_LimitSwitch = 31; // Switch that activates when 1st Main Antenna is homed
const int ant2_LimitSwitch = 33; // Switch that activates when 2nd Main Antenna is homed
const int ref_LimitSwitch = 35; // Switch that activates when Reflector Antennas are homed
const int en_AntPin = 37; // Enable pin for Main Antenna Motor
const int en_RefPin = 39; // Enable pin for Reflector Antenna Motor

// Define Min & Max Frequency
const long freqMIN = 320000000; // 320 MHz
const long freqMAX = 1600000000; // 1.6 GHz

// Define Motor Speed
const int Speed = 1200; // in Steps per Second

// Define Global Variables
unsigned long frequency;  // Frequency Variable
long ant_ReqStep; // Required Steps of Antenna Motor
long ref_ReqStep; // Required Steps of Reflector Antenna Motor

// Variables that hold encoder values
long ant_ENC = 0;  // Positive Values are CCW & Negative Valeus are CW
long ref_ENC = 0;  
                 
void setup() {
  // Setup Pins 
  pinMode(AA_PHASE, INPUT);
  pinMode(AB_PHASE, INPUT);
  pinMode(BA_PHASE, INPUT);
  pinMode(BB_PHASE, INPUT);
  pinMode(ant1_LimitSwitch, INPUT);
  pinMode(ant2_LimitSwitch, INPUT);
  pinMode(ref_LimitSwitch, INPUT);
  pinMode(ant_StepPin, OUTPUT);
  pinMode(ant_DirPin, OUTPUT);
  pinMode(ref_StepPin, OUTPUT);
  pinMode(ref_DirPin, OUTPUT);
  pinMode(en_AntPin, OUTPUT);
  pinMode(en_RefPin, OUTPUT);
  
  // Attach Interrupt to both Rotary Encoders
  attachInterrupt(digitalPinToInterrupt(AA_PHASE), antEncoder, RISING); //Interrupt trigger mode: RISING
  attachInterrupt(digitalPinToInterrupt(BA_PHASE), refEncoder, RISING); //Interrupt trigger mode: RISING

  // Set up Serial Connection
  Serial.begin(115200);
  Serial.setTimeout(50);
  
  // Home in Antenna at starting position
  antHome(); 
}

void loop() {
  // When there is data from Raspberry Pi
  if(Serial.available())
  {
    // Take frequency input from Serial Line
    frequency = Serial.parseInt();

    if(frequency == 0)
    {
      antHome(); // Home Antenna
    }
    else if((frequency >= freqMIN) && (frequency <= freqMAX))
    {
      // Calculate steps needed for Antenna and Reflector Antenna Motors
      ant_ReqStep = ant_StepsCalc(frequency);
      ref_ReqStep = ref_StepsCalc(frequency); 

      // Enable Motors
      enableMotors();
      
      // Move Main Antenna Motor
      moveMotor(ant_ReqStep, ant_StepPin, ant_DirPin, ant_ENC);

      // Move Reflector Antenna Motor
      moveMotor(ref_ReqStep, ref_StepPin, ref_DirPin, ref_ENC);

      // Disable Motors
      disableMotors();

      Serial.write(1); // Send 1: Antenna is at proper length 
    }
    else
    {
      Serial.write(5); // Send 5: indicate that the frequency is not within range
    }
  }
}

/***********************************************************
 * Enable Antennas
************************************************************/
void enableMotors()
{
  digitalWrite(en_AntPin, HIGH);
  digitalWrite(en_RefPin, HIGH);
}

/***********************************************************
 * Disable Antennas
************************************************************/
void disableMotors()
{
  digitalWrite(en_AntPin, LOW);
  digitalWrite(en_RefPin, LOW);
}

/***********************************************************
 * Homes Main Antenna and Reflector Antennas 
************************************************************/
void antHome()
{ 
  // Create AccelStepper Objects
  AccelStepper AntStepper(1, ant_StepPin, ant_DirPin);
  AccelStepper RefStepper(1, ref_StepPin, ref_DirPin);

  // Set Max Speeds
  AntStepper.setMaxSpeed(800);
  RefStepper.setMaxSpeed(800);
  
  // Enable Motors
  enableMotors();
  
  // Home the Main Antenna
  while(!(digitalRead(ant1_LimitSwitch) || digitalRead(ant2_LimitSwitch)))
  {
    AntStepper.move(-1);
    AntStepper.runSpeed();
  }

  // Check that both limit switches of Main Antenna has to be activated
  if(!(digitalRead(ant1_LimitSwitch) && digitalRead(ant2_LimitSwitch)))
  {
    Serial.write(3); // Send 3: There is a mistep in the belt system
    return; 
  }

  // Home the Reflector Antenna
  while(!digitalRead(ref_LimitSwitch))
  {
    RefStepper.move(-1);
    RefStepper.runSpeed();
  }


  // Reset the Encoder Values
  ant_ENC = 0;
  ref_ENC = 0;

  // Disable Motors
  disableMotors();

  // Check if properly homed
  if(digitalRead(ref_LimitSwitch))
  {
    Serial.write(2); // Send 2: antennas homed properly
  }
  else
  {
    Serial.write(5); // Send 5 indicating there is a problem with homing the reflector antennas
  }
}

/***********************************************************
 * Antena Motor Encoder Interrupt
************************************************************/
void antEncoder()
{
  char i;
  i = digitalRead(AB_PHASE);
  if (i==1) // Turning CCW
    ant_ENC += 1;
  else // Turning CW
    ant_ENC -= 1;
}

/***********************************************************
 * Reflector Antenna Motor Encoder Interrupt
************************************************************/
void refEncoder()
{
  char i;
  i = digitalRead(BB_PHASE);
  if (i==1) // Turning CCW
    ref_ENC += 1;
  else // Turning CW
    ref_ENC -= 1;
}

/***********************************************************
 * Converts Frequency into Steps for Antenna
************************************************************/
long ant_StepsCalc(unsigned long freq)
{
  // TODO Calculates steps required from frequency
  // Find Antenna Length Required for input frequency

  // Subtract Antenna Length by Minimum Frequency Length
  
  // Convert resulting length into # of steps 
  
  // Return Steps
}

/***********************************************************
 * Converts Frequency into Steps for Reflector Antenna
************************************************************/
long ref_StepsCalc(unsigned long freq)
{
  // TODO Calculates steps required from frequency
  // Find Reflector Distance Required for input frequency

  // Subtract Antenna Length by Minimum Frequency Distance

  // Convert resulting length into # of steps
  
  // Return Steps
}

/***********************************************************
 * Motor Sequence
 * determines whether motor moves CW or CCW 
 * as well as the encoder closed loop logic
************************************************************/
void moveMotor(long ReqStep, long StepPin, int DirPin, long Encoder)
{
  //Accel Stepper Object
  AccelStepper stepper(1, StepPin, DirPin);
  stepper.setMaxSpeed(Speed);
  
  // Move Motor to required step
  stepper.move(ReqStep - Encoder);
  stepper.setSpeed(Speed);

  // Keep running motor until distance is reached
  while(stepper.distanceToGo() != 0)
  {
    stepper.runSpeedToPosition(); // Command to run at constant speed specified
  }

  // Match Encoder reading to required step
  while(Encoder != ReqStep)
  {
    stepper.setSpeed(800);  // Set at lower speed for precise movements

    while(stepper.distanceToGo() != 0)
    {
      stepper.runSpeedToPosition();
    }
  }
}
