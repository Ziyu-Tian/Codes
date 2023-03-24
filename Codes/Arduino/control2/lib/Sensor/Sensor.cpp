#include "Sesnor.hpp"

struct PID
{
    float Kp;
    float Ki;
    float Kd;
};

PID steerTuning = {0, 0, 0};

PID speedTuning = {0, 0, 0};

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
