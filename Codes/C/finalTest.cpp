#include "CoreNum.hpp"
#include "CpuEfficiency.hpp"
#include "Meminfo.hpp"
#include <fstream>
#include <iostream>
#include "upTime.hpp"
#include "cpuEnergy.hpp"
using namespace std;
int main() {
  while (true) {
   
    cpuPrint();
    memPrint();
    timePrint();
    energyPrint();
    usleep(500000);
    clearScreen();
  }
  return 0;
}