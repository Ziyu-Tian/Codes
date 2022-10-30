#include "CoreNum.hpp"
#include "CpuEfficiency.hpp"
#include "Meminfo.hpp"
#include <fstream>
#include <iostream>
#include "uptime.hpp"
#include "cpuEnergy.hpp"
using namespace std;
int main() {
  while (true) {
   
    cpuPrint();
    memPrint();
    cout<<"------------------------------------------------------------------------"<<endl;
    timePrint();
    energyPrint();
  
    usleep(500000);
    clearScreen();
    // statClose(stat);
    // memClose(mem);
    // statReopen(stat);
    // memReopen(mem);

    // stat.close();
    // mem.close();
  }
  return 0;
}