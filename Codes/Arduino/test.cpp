#include <Arduino.h>
#include <Servo.h>
#define rightSensor A0
#define leftSensor A1
Servo myServo;
struct PID
{
  float Kp;
  float Ki;
  float Kd;
};
int positionPID(float deviation, PID pid);
float sensorRead();

PID steerTuning = {0.374, 7.22324e-05, 0.05};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myServo.attach(6);
  //myServo.write(90);
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  float left = analogRead(leftSensor);
  float right = analogRead(rightSensor);
  float deviation = left- right;
  float PWM = positionPID(deviation, steerTuning);
  Serial.println(PWM);
  myServo.write(PWM);
  delay(25);
  

}
int positionPID(float deviation, PID pid)
{
    float Position_Kp = pid.Kp;
    float Position_Ki = pid.Ki;
    float Position_Kd = pid.Kd;
    static float Bias, PWM, Integral_Bias, Last_Bias;

    Bias = deviation;
    Integral_Bias += Bias;
    PWM = Position_Kp*Bias + Position_Ki*Integral_Bias + Position_Kd*(Bias-Last_Bias);
    Last_Bias = Bias;

    return PWM;
}
float sensorRead()
{
    float difference = (analogRead(leftSensor) -analogRead(rightSensor)) * 5 /1023;
    return difference;
}