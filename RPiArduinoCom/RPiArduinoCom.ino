// Declare frequency variable that will be taken from Raspberry Pi
long frequency;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(50); // Defaults to 1000ms if not set

  Serial.write(2);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  { 
    //Take frequency input from Serial Line
    frequency = Serial.parseInt(); // Converts string into long

    if((frequency == 0) || (frequency == 50000000))
    {
      Serial.write(2); // Sent a message that arduino is complete with its task 
    }
    else if((frequency > 50000000) && (frequency <= 1000000000))
    {
      Serial.write(1);
    }
    else
    {
      Serial.write(3);
    }
  }
}
