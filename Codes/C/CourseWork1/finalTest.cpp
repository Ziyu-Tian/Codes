#include "meminfo.hpp"
#include "stat.hpp"
#include "uptime.hpp"
#include <bits/stdc++.h> // the all-in-one header
#include <iostream>
using namespace std;
int main()
{
  while (true)
  {

    cpuPrint();     // print the cpu data
    memPrint();     // print the memory data
    timePrint();    // print the time
    energyPrint();  // print the energy
    usleep(500000); // suspend the program
    clearScreen();  // clean the screen
  }
  return 0;
}