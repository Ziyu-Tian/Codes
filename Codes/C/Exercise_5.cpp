#include<iostream>
#include<string>
#include<cstring>
using namespace std;
void do_encrypt(char m[]);
void display_message(char m[]);
int main()
{
   char m[100];
   cout<<"Please input your message:"<<endl;
   cin.get(m,100);// cin.get can store the space
   cout<<"The original message:"<<endl;
   display_message(m);
   do_encrypt(m);
   cout<<"The encrypted message:"<<endl;
   display_message(m);
   return 0;
}
void do_encrypt(char m[])
{
    int size = strlen(m)-1;// strlen can calculate the length of character array
    // the '\0' is the last element
    for (int i = 0; i < size; i++)
    {
       m[i] = ((m[i] * 9) % 96) + 31;
    }
}
void display_message(char m[])
{
    
    cout<<m<<endl;
}