/* A Linear Regression Exercise to Solve for "Good Enough" PID control coefficients */
/* Code: RA Shafik | 8 March 2019 */
/* Demonstrated to EEE2007 Students */

#include <iostream>

using namespace std;

#define SAMPLING_TIME 0.0001
#define ALPHA 0.0001
#define ITERATIONS 200

void regress(double *, double *, unsigned int, double &, double &, double &);

int main()
{
    double rotation_data[] = {65, 77.5, 90, 102.5, 115};//Five PWM known PWM outputs from the Microcontroller
    double sensor_difference_data[] = {10, 52, 50, 99, 360};//Five input observations in the form of difference between left and right sensors
 
    double kp = 0; //Proportional co-efficient
    double k0 = 90; //offset PWM value
    double ki = 0; //Integral co-efficient
    
    regress(sensor_difference_data, rotation_data, 5, kp, ki, k0);

   return 0;
}

void regress(double *inputs, double *expected_outputs, unsigned int input_output_size, double &k_p, double &k_i, double &k_0){
	cout << "At learning phase.....(using gradient descent)" << endl;
    cout << "====================================" << endl;
		int idx;
		double predicted_output;
		double error = 0, prev_diff = 0;
    for (int i = 0; i < ITERATIONS; i ++) {
        idx = i % input_output_size;
        predicted_output = k_0 + k_p * inputs[idx] + k_i * ((prev_diff + inputs[idx]) * SAMPLING_TIME); //predicted rotation PWM signal
        error = predicted_output - expected_outputs[idx]; //error between predicted and actual rotation PWM signal
        k_p = k_p - ALPHA * error * inputs[idx]; //updating kp in gradient descent 
        k_i = k_i - ALPHA * error * ((prev_diff + inputs[idx]) * SAMPLING_TIME); //updating ki in gradient descent 
        cout << "iteration " << i << ", k0: " << k_0 << ", kp: " << k_p << ", ki: " << k_i << ", error: " << error << endl; // print debug of the values
        prev_diff = inputs[idx]; //save for previous sensor difference for I controller
    }
}

