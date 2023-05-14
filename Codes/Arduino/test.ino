#include <Arduino.h>
#include <Servo.h>
#define rightSensor A0
#define leftSensor A1
#define PWM_forward_pin 5
Servo myServo;
struct PID
{
  float Kp;
  float Ki;
  float Kd;
};

int positionPID(float deviation, PID pid);
// PID tuning initial stage
// PID steerTuning = {0.45,0.005, 2};// speed 10
// PID steerTuning = {0.45,0.0003, 0};// speed 20
// PID steerTuning = {0.40,0.003, 1};// speed 30
// PID steerTuning = {0.40,0.003, 1};// speed 25
// PID steerTuning = {0.40,0.003, 1};// speed 25, the most stable version

// PID tuning second stage
// PID steerTuning = {0.3,0.003, 0};// speed 20
// PID steerTuning = {0.30,0.006, 0};// speed 25
// PID steerTuning = {0.3,0.03,1};//20
// PID steerTuning = {0.45,0.003,0};//18
// PID steerTuning = {0.48,0.006,0};//20
// PID steerTuning = {0.3,0.006,0};//15 7.9 V

// PID tuning after servo improvement
// PID steerTuning = {0.3,0.003,0};//15 7.7V
// PID steerTuning = {0.31,0.003,0};//15 7.7V
// PID steerTuning = {0.25, 0.0000475, 0}; //30_15 7.7V
// PID steerTuning = {0.3, 0.0000475, 0};//final 22_15
PID steerTuning = {0.3, 0.0000475, 0}; // final 22_15
void setup()
{

  Serial.begin(9600);
  myServo.attach(6);
  myServo.write(90);
  delay(1000000);
  pinMode(PWM_forward_pin, OUTPUT); // set pwm pin as an output
}

void loop()
{

  float duty_cycle;
  float left = analogRead(leftSensor);
  float right = analogRead(rightSensor);
  float deviation = left - right;
  float PWM;

  PWM = positionPID(deviation, steerTuning);

  Serial.print((String) "Left_sensor: " + left); // message for testing on serial monitor
  Serial.println((String) " Right_sensor: " + right);

  float PWM_ANGLE = 90 + PWM;

  // limit judgement
  if (PWM_ANGLE >= 150)
  {
    PWM_ANGLE = 150;
  }
  if (PWM_ANGLE <= 30)
  {
    PWM_ANGLE = 30;
  }
  myServo.write(PWM_ANGLE);
  delay(25);

// speed control cases
  if (abs(deviation) < 15 || abs(deviation) == 15)
  {
    duty_cycle = 20;
  }
  else
  {

    duty_cycle = 15;
  }

  analogWrite(PWM_forward_pin, duty_cycle); // set PWM duty cycle...

  delay(2);
}
// PID function
int positionPID(float deviation, PID pid)
{
  float Position_Kp = pid.Kp;
  float Position_Ki = pid.Ki;
  float Position_Kd = pid.Kd;
  static float Bias, PWM, Integral_Bias, Last_Bias;

  Bias = deviation;
  Integral_Bias += Bias;
  PWM = Position_Kp * Bias + Position_Ki * Integral_Bias + Position_Kd * (Bias - Last_Bias);
  Last_Bias = Bias;

  return PWM;
}