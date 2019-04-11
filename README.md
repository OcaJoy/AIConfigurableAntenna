# AIConfigurableAntenna
The AIConfigurableAtenna is a dipole configruable antenna with 2 reflector antennas. The AIConfigurable Antenna makes use of 2 motors, 1 for extending the main antennas (dipoles) through the use of a belt system, and the other for controlling the distance of the reflector antennas thorugh a gear system. The motors are controlled by an Arduino Mega 2560 REV 3 which is connected to a Raspberry Pi where the AI resides. 

## Getting Started

To run this program, the following software are needed:

- [Python](https://www.python.org/downloads/)

- [Arduino](https://www.arduino.cc/en/main/software): is only required to edit and upload the program to the Arduino Mega

## Programs
### ConfigurableAntenna.ino
This is the main Arduino program that will control the length of the antenna depending on the input from RPiArduinoCom.py

### RPiArduinoCom.py
This is the python program that is uploaded to the Raspberry Pi and will be used by the AI to send digital instructions to the Arduino.

## Libraries
- [PySerial](https://pypi.org/project/pyserial/) [Version 3.4]
  - This library is used to establish a serial connection between python and arduino to be able to read and write data between the two systems.
  
- [AccelStepper](https://www.airspayce.com/mikem/arduino/AccelStepper/) [Version 1.59] 
  - This library is used to send signals to the Pololu A4988 Stepper Motor Driver to be able to drive the Stepper Motors.

## Diagrams and Pictures
All diagram and pictures can be found in docs/source/img

## STL CAD Files
All .stl CAD Files can be found in docs/source/slt
