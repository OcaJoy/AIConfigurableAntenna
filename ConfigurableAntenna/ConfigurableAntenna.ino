// Define Constants for Pins
const int A_PHASE = 19;
const int B_PHASE = 18;
const int C_PHASE = 3;
const int D_PHASE = 2;
const int ant_StepPin = 23;
const int ant_DirPin = 25;
const int ref_StepPin = 27;
const int ref_DirPin = 29;
const int ant_LimitSwitch = 31; // Switch that activates when Main Antennas are homed
const int ref_LimitSwitch = 33; // Switch that activates when Reflector Antennas are homed
const int en_AntPin = 35; // Enable pin for Main Antenna Motor
const int en_RefPin = 37; // Enable pin for Reflector Antenna Motor
const long freqMIN = 10000000;
const long freqMAX = 1000000000;

// Define Global Variables
unsigned long frequency;  // Frequency Variable
long ant_CurStep = 0; // Current Steps of the Antenna Motor
long ref_CurStep = 0; // Current Steps of the Reflector Antenna Motor
long ant_ReqStep; // Required Steps of Antenna Motor
long ref_ReqStep; // Required Steps of Reflector Antenna Motor

// Variables that hold encoder values
long ant_ENC = 0;  // Positive Values are CW & Negative Valeus are CCW
long ref_ENC = 0;  
                 
void setup() {
  // Setup Pins 
  pinMode(A_PHASE, INPUT);
  pinMode(B_PHASE, INPUT);
  pinMode(C_PHASE, INPUT);
  pinMode(D_PHASE, INPUT);
  pinMode(ant_LimitSwitch, INPUT);
  pinMode(ref_LimitSwitch, INPUT);
  pinMode(ant_StepPin, OUTPUT);
  pinMode(ant_DirPin, OUTPUT);
  pinMode(ref_StepPin, OUTPUT);
  pinMode(ref_DirPin, OUTPUT);
  pinMode(en_AntPin, OUTPUT);
  pinMode(en_RefPin, OUTPUT);
  
  // Attach Interrupt to both Rotary Encoders
  attachInterrupt(digitalPinToInterrupt(A_PHASE), antEncoder, RISING); //Interrupt trigger mode: RISING
  attachInterrupt(digitalPinToInterrupt(C_PHASE), refEncoder, RISING); //Interrupt trigger mode: RISING

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
      ant_ReqStep = ant_StepsCalc();
      ref_ReqStep = ref_StepsCalc(); 

      // Enable Motors
      enableMotors();
      
      // Move Main Antenna Motor
      moveMotor(ant_ReqStep, ant_CurStep, ant_DirPin, ant_ENC);

      // Move Reflector Antenna Motor
      moveMotor(ref_ReqStep, ref_CurStep, ref_DirPin, ref_ENC);

      // Disable Motors
      disableMotors();

      Serial.write(1);
    }
    else
    {
      Serial.write(3); // Send 3: indicate that the frequency is not within range
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
  // Enable Motors
  enableMotors();
  
  // Home the Main Antenna
  while(!digitalRead(ant_LimitSwitch))
  {
    // TODO Slowly Move motor back until the Limit Switch of Main Antenna is actuated  
  }

  // Reset Current Steps of the Main Antenna Variable
  long ant_CurStep = 0;

  // Home the Reflector Antenna
  while(!digitalRead(ref_LimitSwitch))
  {
    // TODO Slowly Move motor back until the Limit Switch of Reflector Antenna is actuated
  }

  // Reset Current Steps of the Reflector Antenna Variable
  long ref_CurStep = 0;

  // Disable Motors
  disableMotors();

  // Check if properly homed
  if(digitalRead(ant_LimitSwitch) && digitalRead(ref_LimitSwitch))
  {
    Serial.write(2); // Send 2: antennas homed properly
  }
  else
  {
    Serial.write(0); // Send 0 indicating there is a problem with homing
  }
}

/***********************************************************
 * Antena Motor Encoder Interrupt
************************************************************/
void antEncoder()
{
  char i;
  i = digitalRead(B_PHASE);
  if (i==1) // Turning CCW
    ant_ENC -= 1;
  else // Turning CW
    ant_ENC += 1;
}

/***********************************************************
 * Reflector Antenna Motor Encoder Interrupt
************************************************************/
void refEncoder()
{
  char i;
  i = digitalRead(B_PHASE);
  if (i==1) // Turning CCW
    ref_ENC -= 1;
  else // Turning CW
    ref_ENC += 1;
}

/***********************************************************
 * Converts Frequency into Steps for Antenna
************************************************************/
long ant_StepsCalc()
{
  // TODO Calculates steps required from frequency
  // Return Steps
}

/***********************************************************
 * Converts Frequency into Steps for Reflector Antenna
************************************************************/
long ref_StepsCalc()
{
  // TODO Calculates steps required from frequency
  // Return Steps
}

/***********************************************************
 * Motor Sequence
 * determines whether motor moves CW or CCW 
 * as well as the encoder closed loop logic
************************************************************/
void moveMotor(long ReqStep, long CurStep, int Dir, long Encoder)
{

  // Move Motor to required step
  if(ReqStep > CurStep)
  {
    digitalWrite(Dir, HIGH); //Turn Motor CW
    // stepMotor(ReqStep - CurStep);    
  }
  else if (ReqStep < CurStep)
  {
    digitalWrite(Dir, LOW);  //Turn Motor CCW
    // stepMotor(CurStep - ReqStep);
  }
  else
  {
    // Do nothing if equal
  }

  // Match Encoder reading to required step
  while(Encoder != ReqStep) // As long as encoder detects motor understep/overstep, motor will keep adjusting until it reaches required steps
  {
    if(Encoder > ant_ReqStep)
    {
      digitalWrite(ant_DirPin, LOW);  //Turn Motor CCW
      //stepMotor(Encoder - ReqStep)
    }
    else
    {
      digitalWrite(ant_DirPin, HIGH); //Turn Motor CW
      //stepMove(ReqStep - Encoder);
    }
  }

  // Set new step position as current step
  CurStep = Encoder;
}


/***********************************************************
 * Step Motor
 * moves the motor in steps
************************************************************/
void stepMotor(long steps)
{
  // TODO Move motor with proper rpm and acceleration
}
