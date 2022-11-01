#include "stat.hpp"
#include "uptime.hpp"
#include "meminfo.hpp"
#include <iostream>
#include <bits/stdc++.h>
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