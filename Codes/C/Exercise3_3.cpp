#include <iostream>
#include <string>
#include <fstream>
using namespace std;
double P_1(double v,int f);
int main()
{
    ofstream data;
    data.open("power.csv");
    double Low_V, High_V, increment;
    
    cout << "Please input the lowest voltage (>0.3V):" << endl;
    cin >> Low_V;
    cout << "Please input the highest voltage (<1.5V):" << endl;
    cin >> High_V;
    cout << "Please input the increment:" << endl;
    cin >> increment;
    double current_V = Low_V;
    data << "Current Voltage,"
         << "Power@20MHz,"
         << "Power@40MHz,"
         << "Power@60MHz,"
         << "Power@80MHz,"
         << "Power@100MHz"
         << endl;
    while (current_V<=High_V)
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
double P_1(double v,int f)
{
   return v*v*f;
}