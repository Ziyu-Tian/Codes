#include <iostream>
#include <string>
using namespace std;
int main()
{
    string s;
    string s_1 = "";// a new string to store the sentence
    cout<<"Please input the message:";
    getline(cin, s);
    int size = s.length();//get the length of the string 
    for (int i = 0; i<size; i++)
    {
        if (s[i] != ' ')//not space, append that character to s_1
        {
            s_1.append(1,s[i]);
            // append(size_t n, char c) is the prototype, n is the number of the char 
        }
            
        else if (s[i + 1] != ' ') // check if next bit is space
            s_1.append(1,s[i]);
            // if the next is also space, i++ until s[i+1] is not space
            // if the next is not space, the previous space will be appended
    }
    cout<<s_1<<endl;
    return 0;
}
