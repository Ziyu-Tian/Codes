#include <Arduino.h>
#include <Servo.h>
#define rightSensor A0
#define leftSensor A1

// MOTOR TESTING
#define PWM_forward_pin 5
int duty_cycle = 0;
//

Servo myServo;
struct PID
{
  float Kp;
  float Ki;
  float Kd;
};
int positionPID(float deviation, PID pid);
float sensorRead();
float left = analogRead(leftSensor);
float right = analogRead(rightSensor);
//PID steerTuning = {0.45,0.005, 2};// speed 10
//PID steerTuning = {0.45,0.0003, 0};// speed 20
//PID steerTuning = {0.40,0.003, 1};// speed 30
//PID steerTuning = {0.40,0.003, 1};// speed 25
//PID steerTuning = {0.40,0.003, 1};
PID steerTuning = {0.20,0.001, 1};// speed 35
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myServo.attach(6);
  delay(50);
  myServo.write(90);
  delay(100);
  //myServo.write(180);
  
  // MOTOR TESTING
  
  pinMode(PWM_forward_pin, OUTPUT); //set pwm pin as an output...
  //pinMode(PWM_reverse_pin, OUTPUT);
  
  //Serial.begin(9600); // initialise serial comm...
  //
  


}

void loop() {
  // put your main code here, to run repeatedly:
  float left = analogRead(leftSensor);
  float right = analogRead(rightSensor);
  float deviation = left - right;
  float PWM;
  float PWM_ANGLE;

  /*
    if (deviation > 3)
    {
    PWM = positionPID(deviation, steerTuning);
    }
    else
    {
    PWM = positionPID(last_deviation, steerTuning);
    }
  */

  PWM = positionPID(deviation, steerTuning);
  //map(value, fromLow, fromHigh, toLow, toHigh)
  PWM_ANGLE=PWM;
  /*
  if(PWM_ANGLE>=130){
    PWM_ANGLE=130;
  }
  if(PWM_ANGLE<=65){
    PWM_ANGLE=65;
  }
  */

  //Serial.println(deviation);
  
    //Serial.print(left);
    //Serial.print(" ");
    //Serial.println(right);
  
  //Serial.print(10);
  myServo.write(PWM_ANGLE);
  delay(25);
        Serial.println(PWM_ANGLE);

    analogWrite(PWM_forward_pin, duty_cycle); // set PWM duty cycle...
    duty_cycle =35; // increment duty cycle by 5...
  

}
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
float sensorRead()
{
  float difference = (analogRead(leftSensor) - analogRead(rightSensor));
  return difference;
}