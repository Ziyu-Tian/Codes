#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <unistd.h>
#include <string.h>
#include <iomanip> // library used to set precision
#include "stat.hpp"

using namespace std;

void dataPrint(long double uptime, long double idletime);

void timePrint(void);

void energyPrint(void);