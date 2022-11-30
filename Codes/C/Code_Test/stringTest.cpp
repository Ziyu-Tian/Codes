#include<bits/stdc++.h>
using namespace std;
//string stringProcess(string s);
int main()
{
    string s = "abcde";
    //char a[10] = "abcde";
    char* ptr;
    ptr = &s[0];
    *ptr = '1';
    cout << *ptr <<endl;
    return 0;

}



/*
string stringProcess(string s)
{
    int length = s.length();
    char* ptr;
    
}
*/