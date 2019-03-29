# AIConfigurableAntenna

## ConfigurableAntenna
This is the main Arduino program that will control the length of the antenna depending on the input from RPiArduinoCom.py

## RPiArduinoCom
This is the python program that is uploaded to the Raspberry Pi and will be used by the AI to send digital instructions to the Arduino.

## Libraries
⋅⋅* PySerial [Version 3.4]
⋅⋅⋅ This library is used to establish a serial connection between python and arduino to be able to read and write data between the two systems.
⋅⋅* AccelStepper [Version 1.59] 
⋅⋅⋅ This library is used to send signals to the Pololu A4988 Stepper Motor Driver to be able to drive the Stepper Motors.
