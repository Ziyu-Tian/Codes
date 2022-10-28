#include <iostream>
#include "CoreNum.hpp"
#include "CpuEfficiency.hpp"
#include "Meminfo.hpp"
#include "uptime.hpp"
#include "cpuEnergy.hpp"
using namespace std;
int main()
{
    printCore();
    cpuPrint();

    return 0;
}