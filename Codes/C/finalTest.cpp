#include <bits/stdc++.h>
#include "CoreNum.hpp"
#include "CpuEfficiency.hpp"
#include "Meminfo.hpp"
#include "uptime.hpp"
#include "cpuEnergy.hpp"
using namespace std;
int main()
{
    ifstream stat("/proc/stat");
    ifstream mem("/proc/meminfo");

    memPrint(mem);
    cpuPrint(stat);
    // usleep(500000);
    // clearScreen();
    // statClose(stat);
    // memClose(mem);
    // statReopen(stat);
    // memReopen(mem);

    //stat.close();
    //mem.close();

    return 0;
}
