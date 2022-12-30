#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
using namespace std;
float P_1(float v,int f);// the power calculation function 
int main(int argc, char *argv[]) // the main function parameters
{
    ofstream data;
    data.open("power.csv");// open the target file 
    float Low_V, High_V, increment = 0;

    string high_string, low_string, increment_string;
    stringstream convert;
    // the argv[0] is the file name and address
    low_string = string(argv[1]);
    high_string = string(argv[3]);
    increment_string = string(argv[5]);
    if (low_string == "-lowvdd")
    {
        convert << argv[2];
        convert >> Low_V;
        convert.clear();
    }
    if (high_string == "-highvdd")
    {
        convert << argv[4];
        convert >> High_V;
        convert.clear();
    }
    if (increment_string == "-step")
    {
        convert << argv[6];
        convert >> increment;
        convert.clear();
    }
    
    float current_V = Low_V;
    data << "Current Voltage,"
         << "Power@20MHz,"
         << "Power@40MHz,"
         << "Power@60MHz,"
         << "Power@80MHz,"
         << "Power@100MHz"
         << endl;
    // print the titles 
    while (current_V<High_V || current_V == High_V)
    {

        data<<current_V<<",";
        data<<P_1(current_V,20)<<","
            <<P_1(current_V,40)<<","
            <<P_1(current_V,60)<<","
            <<P_1(current_V,80)<<","
            <<P_1(current_V,100)<<endl;
        current_V += increment;
    }


    return 0;
}
float P_1(float v,int f)
{
   return v*v*f;
}

