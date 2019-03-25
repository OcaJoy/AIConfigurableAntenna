// Declare frequency variable that will be taken from Raspberry Pi
long frequency;
long freqMIN = 320000000;   // 320 MHz 
long freqMAX = 1600000000;  // 1.6 GHz

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

    if((frequency == 0) || (frequency == freqMIN))
    {
      Serial.write(2); // Sent a message that arduino is complete with its task 
    }
    else if((frequency > freqMIN) && (frequency <= freqMAX))
    {
      Serial.write(1);
    }
    else
    {
      Serial.write(5);
    }
  }
}
