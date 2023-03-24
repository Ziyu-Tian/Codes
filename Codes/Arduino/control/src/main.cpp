#include <Arduino.h>
#include <Servo.h>
#include <EEPROM.h>
#include "Sesnor.hpp"
Servo myServo;
#define rightSensor 12
#define leftSensor 11
#define servoPWM 9
#define buttonPin 8 // the button connected to the Digital_8 with the pull_down resistor to GND
int addressROM = 0;
void interruptWrite();
struct PID
{
  float Kp;
  float Ki;
  float Kd;
};
void setup()
{
  // put your setup code here, to run once:
  myServo.attach(servoPWM);
  //Serial.begin(9600);
  pinMode(leftSensor, INPUT);
  pinMode(rightSensor, INPUT);
  pinMode(buttonPin,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), interruptWrite, CHANGE);
}

void loop()
{
  // put your main code here, to run repeatedly:


  // read the difference between the left and right sensor 

  /*
  myServo.writeMicroseconds(1200);
  delay(2000);
  myServo.writeMicroseconds(1500);
  delay(2000);
  myServo.writeMicroseconds(1800);
  delay(2000);
  */
}

void interruptWrite()
{ 
  float deviation = sensorRead();
  EEPROM.write(addressROM,deviation);
  if (addressROM <= 512)
  {
    addressROM += 1;
  }
  
}