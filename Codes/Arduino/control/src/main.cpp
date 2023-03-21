#include <Arduino.h>
#include <Servo.h>
#include "Sesnor.hpp"
Servo myServo;
#define rightSensor A0
#define leftSensor A1
#define servoPWM 9
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
}

void loop()
{
  // put your main code here, to run repeatedly:

  float deviation = sensorRead();
  // read the difference between the left and right sensor 

  /*
  myServo.writeMicroseconds(1200);
  delay(2000);
  myServo.writeMicroseconds(1500);
  delay(2000);
  myServo.writeMicroseconds(1800);
  delay(2000);
  */
 
  Serial.println(deviation);
  delay(1000);
}