#include "meminfo.hpp"
void memPrint()
{
    ifstream mem("/proc/meminfo");
    string line;
    if (!mem.good())
    {
        cerr << "Could not open file.... exitting..." << endl;
        exit(EXIT_FAILURE);
    }
    // the file ability check 
  
    long double Total = 0;
    long double Free = 0;
    long double Buffers = 0;
    long double Cached = 0;
    while (getline(mem, line))
    {
        stringstream linestream(line);
        string token;
        getline(linestream, token, ':'); // token is the first word before ':'

        if (token == "MemTotal")
        {
            linestream >> Total;
            Total = Total / 1024; // the unit conversion 
        }
        if (token == "MemFree")
        {
            linestream >> Free;
            Free = Free / 1024;
        }
        if (token == "Buffers")
        {
            linestream >> Buffers;
            Buffers = Buffers / 1024;
        }
        if (token == "Cached")
        {
            linestream >> Cached;
            Cached = Cached / 1024;
        }
    }

    cout << "-----------------------------------------------------------------------"
         << endl
         << "MEMORY \t"
         << "Total: " << fixed << setprecision(0) // set the precision 
         << Total << " MB" << endl
         << "      \t"
         << "Free: " << Free << " MB" << endl
         << "      \t"
         << "Cached: " << Cached << " MB" << endl
         << "      \t"
         << "Buffers: " << Buffers << " MB" << endl;
      cout<<"------------------------------------------------------------------------"<<endl;

            mem.close(); // close the file and reopen to refresh
            mem.open("/proc/meminfo");
            if (!mem.good())
            {
                cerr << "Could not open file.... exitting..." << endl;
                exit(EXIT_FAILURE);
            }
    
     mem.close();
}
